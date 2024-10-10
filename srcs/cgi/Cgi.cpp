#include "Cgi.hpp"
#include "logging.hpp"
#include <poll.h>

Cgi::Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Trying out RequestPacket getters:\ngetHttpVersion: " + request_packet.getHttpVersion() + "\ngetUri: " + request_packet.getUri() + "\ngetMethod: " + std::to_string(request_packet.getMethod()) + "\ngetContentLengthHeader: " + std::to_string(request_packet.getContentLengthHeader()) + "\nisChunked: " + std::string(1, request_packet.isChunked() ? '1' : '0') + "\ngetContentSize: " + std::to_string(request_packet.getContentSize()) + "\ngetQueryString: " + request_packet.getQueryString());
	_path_info = config_pair.second.getRoot() + request_packet.getUri();
	DEBUG("Script path: " + _path_info);
	if (access(_path_info.c_str(), X_OK) == -1)
	{
		ERROR("Cgi script not found");
		return;
	}
	DEBUG("Cgi script found");
	std::vector<std::string> methods = {"GET", "POST", "DELETE", "PUT"};
	_env.push_back("CONTENT_TYPE=" + request_packet.getHeader("Content-Type"));
	_env.push_back("CONTENT_LENGTH=" + std::to_string(request_packet.getContentSize()));
	_env.push_back("HTTP_COOKIE=" + request_packet.getHeader("Cookie"));
	_env.push_back("REQUEST_METHOD=" + methods[request_packet.getMethod()]);
	_env.push_back("QUERY_STRING=" + request_packet.getQueryString()); // TODO: create get_query_string
	_env.push_back("SERVER_PROTOCOL=" + request_packet.getHttpVersion());
	_env.push_back("REMOTE_ADDR=" + request_packet.getHeader("Host"));
	std::vector<std::string>
		server_names = config_pair.first.getServerNames();
	if (!server_names.empty())
		_env.push_back("SERVER_NAME=" + server_names[0]);
	else
		_env.push_back("SERVER_NAME=localhost"); // Default to "localhost" if no server name is defined
	_env.push_back("SERVER_PORT=" + std::to_string(config_pair.first.getPort()));
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	for (auto &e : _env)
		DEBUG(e);
	if (pipe(_fds) == -1)
	{
		ERROR("Pipe failed");
		return;
	}
	(void)response_packet;
	writeToPipe(request_packet.getContent(), _fds[1]);
	execute();
}

void Cgi::writeToPipe(const std::string &content, int fd)
{
	// Poll setup for writing POST data to CGI process
	struct pollfd pollfds[2];
	pollfds[0].fd = fd; // Monitor CGI input pipe for writing
	pollfds[0].events = POLLOUT;

	size_t written = 0;

	while (written < content.size())
	{
		int ret = poll(pollfds, 1, 2000); // Poll for the input pipe being ready

		if (ret < 0)
			throw std::runtime_error("Poll failed on CGI input pipe");
		else if (ret == 0)
			continue; // Timeout, no data to write

		if (pollfds[0].revents & POLLOUT) // Ready to write
		{
			ssize_t bytes_written = write(fd, content.c_str() + written, content.size() - written);
			if (bytes_written < 0)
				throw std::runtime_error("Failed to write to CGI input pipe");

			written += bytes_written; // Update bytes written
		}
	}
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
		int ret = poll(pollfds, 1, -1); // Poll indefinitely until there's data or EOF

		if (ret < 0)
			throw std::runtime_error("Poll failed on CGI output pipe");

		if (pollfds[0].revents & POLLIN) // Ready to read
		{
			bytes_read = read(fd, buffer, sizeof(buffer));
			if (bytes_read < 0)
				throw std::runtime_error("Failed to read from CGI output pipe");
			else if (bytes_read == 0)
				break; // End of file (EOF), child process is done writing

			output.append(buffer, bytes_read); // Append the chunk to the output string
		}
		if (pollfds[0].revents & POLLHUP) // Writing end of the pipe closed
			break;						  // Child process has closed its side of the pipe
	}
	DEBUG("Output from CGI: " + output);

	return output; // Return the full output of the CGI process
}

void Cgi::execute()
{
	int out_fds[2];

	if (pipe(out_fds) == -1)
	{
		ERROR("Pipe failed");
		return;
	}

	pid_t pid = fork();
	if (pid == 0)
	{
		DEBUG("Child process");
		// input
		close(_fds[1]);
		if (dup2(_fds[0], STDIN_FILENO) == -1)
		{
			ERROR("Dup2 failed with stdin redirection in child process");
			exit(1);
		}
		close(_fds[0]);
		// output
		close(out_fds[0]);
		if (dup2(out_fds[1], STDOUT_FILENO) == -1)
		{
			ERROR("Dup2 failed with stdout redirection in child process");
			exit(1);
		}
		close(out_fds[1]);
		// DEBUG("Redirected stdin and stdout of child process");
		// make null terminated env
		std::vector<char *> env;
		for (auto &e : _env)
			env.push_back(const_cast<char *>(e.c_str()));
		env.push_back(nullptr);
		// DEBUG("Execve in child process");
		// execute the cgi script
		char *const argv[] = {const_cast<char *>(_path_info.c_str()), nullptr};
		// DEBUG("Path info: " + _path_info);
		execve(_path_info.c_str(), argv, env.data());
		ERROR("Execve failed");
		exit(1); // if execve fails
	}
	else if (pid > 0)
	{
		DEBUG("Parent process");
		DEBUG("Waiting for child process to finish");

		int status;
		int wait_result;
		int timeout = 5; // seconds
		int elapsed_time = 0;

		close(_fds[0]);
		close(_fds[1]);
		close(out_fds[1]);

		// Wait in intervals of 1 second to allow for a timeout
		while ((wait_result = waitpid(pid, &status, WNOHANG)) == 0 && elapsed_time < timeout)
		{
			sleep(1); // sleep for 1 second
			elapsed_time++;
		}

		if (wait_result == 0) // Timeout occurred
		{
			ERROR("Child process timed out");
			kill(pid, SIGKILL);		  // Force kill the child process
			waitpid(pid, &status, 0); // Ensure the child process is cleaned up
		}
		else if (wait_result > 0) // Child process exited
		{
			_cgi_response = readFromPipe(out_fds[0]);
			DEBUG("Child process finished");
		}
		else // Error occurred
		{
			ERROR("Error in waitpid");
		}
		close(out_fds[0]);
	}
	else
		ERROR("Fork failed");
	DEBUG("execution done");
}

std::string Cgi::getResponse() const
{
	return _cgi_response;
}