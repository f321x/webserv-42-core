#include "Cgi.hpp"
#include "logging.hpp"

Cgi::Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	std::string script_path = config_pair.second.getRoot() + request_packet.getUri();
	DEBUG("Script path: " + script_path);
	if (access(script_path.c_str(), X_OK) == -1)
	{
		ERROR("Cgi script not found");
		return;
	}
	std::vector<std::string> methods = {"GET", "POST", "DELETE", "PUT"};
	_env.push_back("REQUEST_METHOD=" + methods[request_packet.getMethod()]);
	_env.push_back("QUERY_STRING=" + request_packet.getQueryString()); // TODO: create get_query_string
	_env.push_back("CONTENT_LENGTH=" + std::to_string(request_packet.getContentSize()));
	_env.push_back("CONTENT_TYPE=" + request_packet.getContentType());
	_env.push_back("SERVER_PROTOCOL=" + request_packet.getHttpVersion());
	_env.push_back("SERVER_NAME=" + config_pair.first.getServerName());
	_env.push_back("SERVER_PORT=" + std::to_string(config_pair.first.getPort()));
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	if (pipe(_fds) == -1)
	{
		ERROR("Pipe failed");
		return;
	}
	if (write(_fds[1], request_packet.get_content().c_str(), request_packet.getContentSize()) == -1)
	{
		ERROR("Write failed");
		return;
	}
}

void Cgi::execute()
{
	pid_t pid = fork();

	if (pid == 0)
	{
		// child process
		close(_fds[1]);
		dup2(_fds[0], STDIN_FILENO);
		close(_fds[0]);

		// make null terminated env
		std::vector<char *> env;
		for (auto &e : _env)
			env.push_back(const_cast<char *>(e.c_str()));
		env.push_back(nullptr);

		// execute the cgi script
		char *const argv[] = {const_cast<char *>(_path_info.c_str()), nullptr};
		execve(_path_info.c_str(), argv, env.data());
		exit(1); // if execve fails
	}
	else if (pid > 0)
	{
		// parent process
		close(_fds[0]);
		close(_fds[1]);
		int status;
		waitpid(pid, &status, 0);
	}
	else
		ERROR("Fork failed");
}