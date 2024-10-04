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

void Cgi::execute()
{
	pid_t pid = fork();
	// int out_fds[2];

	// if (pipe(out_fds) == -1)
	// {
	// 	ERROR("Pipe failed");
	// 	return;
	// }

	if (pid == 0)
	{
		DEBUG("Child process");
		// input
		close(_fds[1]);
		dup2(_fds[0], STDIN_FILENO);
		close(_fds[0]);
		// output
		// close(out_fds[0]);
		// dup2(out_fds[1], STDOUT_FILENO);
		// close(out_fds[1]);
		DEBUG("Redirected stdin and stdout of child process");
		// make null terminated env
		std::vector<char *> env;
		for (auto &e : _env)
			env.push_back(const_cast<char *>(e.c_str()));
		env.push_back(nullptr);
		DEBUG("Execve in child process");
		// execute the cgi script
		char *const argv[] = {const_cast<char *>(_path_info.c_str()), nullptr};
		DEBUG("Path info: " + _path_info);
		execve(_path_info.c_str(), argv, env.data());
		ERROR("Execve failed");
		exit(1); // if execve fails
	}
	else if (pid > 0)
	{
		DEBUG("Parent process");
		// parent process
		close(_fds[0]);
		close(_fds[1]);
		// close(out_fds[1]);
		// close(out_fds[0]);
		DEBUG("Waiting for child process to finish");
		int status;
		waitpid(pid, &status, 0);
	}
	else
		ERROR("Fork failed");
}
