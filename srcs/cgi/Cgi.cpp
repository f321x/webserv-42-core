#include "Cgi.hpp"
#include "logging.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <sys/stat.h>
#include <signal.h>

Cgi::Cgi(const RequestPacket &request_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
	: _cgi_response(""), _pid(-1)
{
	DEBUG("Initializing CGI processing");

	// Construct the script path
	_path_info = config_pair.second.getRoot() + request_packet.getUri();
	DEBUG("Script path: " + _path_info);

	// Check if the script exists and is executable
	struct stat path_stat;
	if (access(_path_info.c_str(), F_OK | X_OK) != 0 || stat(_path_info.c_str(), &path_stat) != 0 || !S_ISREG(path_stat.st_mode))
		throw std::runtime_error("Not a executable file!");
	DEBUG("CGI script found and is executable");

	// Set up environment variables
	std::vector<std::string> methods = {"GET", "POST", "DELETE", "PUT"};
	_env.push_back("CONTENT_TYPE=" + request_packet.getHeader("Content-Type"));
	_env.push_back("CONTENT_LENGTH=" + std::to_string(request_packet.getContentSize()));
	_env.push_back("HTTP_COOKIE=" + request_packet.getHeader("Cookie"));
	_env.push_back("REQUEST_METHOD=" + methods[request_packet.getMethod()]);
	_env.push_back("QUERY_STRING=" + request_packet.getQueryString());
	_env.push_back("SERVER_PROTOCOL=" + request_packet.getHttpVersion());
	_env.push_back("REMOTE_ADDR=" + request_packet.getHeader("Host"));

	// Server name
	std::vector<std::string> server_names = config_pair.first.getServerNames();
	if (!server_names.empty())
		_env.push_back("SERVER_NAME=" + server_names[0]);
	else
		_env.push_back("SERVER_NAME=localhost");

	_env.push_back("SERVER_PORT=" + std::to_string(config_pair.first.getPort()));
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	// Log environment variables
	for (auto &e : _env)
		DEBUG(e);

	// Create input and output pipes BEFORE forking
	if (pipe(_input_pipe) == -1)
		throw std::runtime_error("Failed to create input pipe");

	if (pipe(_output_pipe) == -1)
	{
		close(_input_pipe[0]);
		close(_input_pipe[1]);
		throw std::runtime_error("Failed to create output pipe");
	}
}

Cgi::~Cgi()
{
	close(_input_pipe[0]);
	close(_input_pipe[1]);
	close(_output_pipe[0]);
	close(_output_pipe[1]);
}

void Cgi::execute(const RequestPacket &request_packet)
{
	DEBUG("Forking process for CGI execution");
	_pid = fork();

	if (_pid < 0)
	{
		ERROR("Fork failed");
		close(_input_pipe[0]);
		close(_input_pipe[1]);
		close(_output_pipe[0]);
		close(_output_pipe[1]);
		return;
	}

	if (_pid == 0)
	{
		if (dup2(_input_pipe[0], STDIN_FILENO) == -1)
		{
			ERROR("dup2 failed for stdin in child process: " + std::string(strerror(errno)));
			exit(1);
		}
		// Redirect stdout to write to _output_pipe[1]
		if (dup2(_output_pipe[1], STDOUT_FILENO) == -1)
		{
			ERROR("dup2 failed for stdout in child process: " + std::string(strerror(errno)));
			exit(1);
		}
		// Close unused file descriptors in the child
		close(_input_pipe[0]);
		close(_input_pipe[1]);
		close(_output_pipe[0]);
		close(_output_pipe[1]);

		// Prepare environment variables as null-terminated array
		std::vector<char *> env;
		for (auto &e : _env)
			env.push_back(const_cast<char *>(e.c_str()));
		env.push_back(nullptr);

		// Prepare arguments & execute
		char *const argv[] = {const_cast<char *>(_path_info.c_str()), nullptr};
		execve(_path_info.c_str(), argv, env.data());

		// If execve returns, it failed
		ERROR("execve failed in child process: " + std::string(strerror(errno)));
		exit(1);
	}
	else
	{
		close(_input_pipe[0]);
		close(_output_pipe[1]);

		// Write the request body to the child's stdin
		// Non-blocking write with timeout
		try
		{
			writeToPipe(request_packet.getContent(), _input_pipe[1]);
		}
		catch (const std::exception &e)
		{
			ERROR("Error writing to CGI input pipe: " + std::string(e.what()));
			// Optionally, kill the child process
			kill(_pid, SIGKILL);
			waitpid(_pid, nullptr, 0);
			close(_input_pipe[1]);
			close(_output_pipe[0]);
			return;
		}
		close(_input_pipe[1]);
		try
		{
			_cgi_response = readFromPipe(_output_pipe[0]);
		}
		catch (const std::exception &e)
		{
			kill(_pid, SIGKILL);
			waitpid(_pid, nullptr, 0);
			close(_output_pipe[0]);
			throw std::runtime_error("Error reading from CGI output pipe: " + std::string(e.what()));
		}
		close(_output_pipe[0]);

		int status;
		int wait_result;
		int timeout = 5; // seconds
		int elapsed_time = 0;

		while ((wait_result = waitpid(_pid, &status, WNOHANG)) == 0 && elapsed_time < timeout)
		{
			sleep(1); // sleep for 1 second
			elapsed_time++;
		}

		if (wait_result == 0)
		{
			ERROR("Child process timed out");
			kill(_pid, SIGKILL);
			waitpid(_pid, &status, 0);
		}
		else if (wait_result > 0)
			DEBUG("Child process finished with status: " + std::to_string(status));
		else
			ERROR("Error in waitpid: " + std::string(strerror(errno)));

		DEBUG("CGI execution completed");
	}
}

void Cgi::writeToPipe(const std::string &content, int fd)
{
	struct pollfd pollfds[1];
	pollfds[0].fd = fd; // Monitor CGI input pipe for writing
	pollfds[0].events = POLLOUT;

	size_t written = 0;
	size_t total = content.size();
	const char *data = content.c_str();

	while (written < total)
	{
		int ret = poll(pollfds, 1, 2000); // Poll with 2-second timeout

		if (ret < 0)
			throw std::runtime_error("Poll failed on CGI input pipe: " + std::string(strerror(errno)));
		else if (ret == 0)
			continue; // Timeout, retry

		if (pollfds[0].revents & POLLOUT)
		{
			ssize_t bytes_written = write(fd, data + written, total - written);
			if (bytes_written < 0)
				throw std::runtime_error("Failed to write to CGI input pipe: " + std::string(strerror(errno)));

			written += bytes_written;
		}
	}

	DEBUG("Successfully wrote " + std::to_string(written) + " bytes to CGI input pipe");
}

std::string Cgi::readFromPipe(int fd)
{
	std::string output;
	char buffer[1024];
	ssize_t bytes_read;

	// Poll setup for reading CGI output
	struct pollfd pollfds[1];
	pollfds[0].fd = fd; // Monitor CGI output pipe for reading
	pollfds[0].events = POLLIN;

	while (true)
	{
		int ret = poll(pollfds, 1, 5000); // 5-second timeout

		if (ret < 0)
			throw std::runtime_error("Poll failed on CGI output pipe: " + std::string(strerror(errno)));
		else if (ret == 0)
			break; // Timeout, stop reading

		if (pollfds[0].revents & POLLIN)
		{
			bytes_read = read(fd, buffer, sizeof(buffer));
			if (bytes_read < 0)
				throw std::runtime_error("Failed to read from CGI output pipe: " + std::string(strerror(errno)));
			else if (bytes_read == 0)
				break; // EOF

			output.append(buffer, bytes_read);
		}

		if (pollfds[0].revents & POLLHUP)
			break; // Pipe closed
	}

	DEBUG("Captured CGI output: \n" + output);
	return output;
}

std::string Cgi::getResponse() const
{
	return _cgi_response;
}
