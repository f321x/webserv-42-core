#include "Cgi.hpp"
#include "logging.hpp"

Cgi::Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Trying out RequestPacket getters:\ngetHttpVersion: " + request_packet.getHttpVersion() + "\ngetUri: " + request_packet.getUri() + "\ngetMethod: " + std::to_string(request_packet.getMethod()) + "\ngetContentLengthHeader: " + std::to_string(request_packet.getContentLengthHeader()) + "\nisChunked: " + std::string(1, request_packet.isChunked() ? '1' : '0') + "\ngetContentSize: " + std::to_string(request_packet.getContentSize()) + "\ngetQueryString: " + request_packet.getQueryString());
	std::string script_path = config_pair.second.getRoot() + request_packet.getUri();
	DEBUG("Script path: " + script_path);
	if (access(script_path.c_str(), X_OK) == -1)
	{
		ERROR("Cgi script not found");
		return;
	}
	DEBUG("Cgi script found");
	std::vector<std::string> methods = {"GET", "POST", "DELETE", "PUT"};
	_env.push_back("REQUEST_METHOD=" + methods[request_packet.getMethod()]);
	_env.push_back("QUERY_STRING=" + request_packet.getQueryString()); // TODO: create get_query_string
	_env.push_back("CONTENT_LENGTH=" + std::to_string(request_packet.getContentSize()));
	std::string content_type = request_packet.getHeader("Content-Type");
	if (content_type.empty())
		content_type = "";
	_env.push_back("CONTENT_TYPE=" + content_type);
	_env.push_back("SERVER_PROTOCOL=" + request_packet.getHttpVersion());
	std::vector<std::string> server_names = config_pair.first.getServerNames();
	if (!server_names.empty())
		_env.push_back("SERVER_NAME=" + server_names[0]);
	else
		_env.push_back("SERVER_NAME=localhost"); // Default to "localhost" if no server name is defined
	_env.push_back("SERVER_PORT=" + std::to_string(config_pair.first.getPort()));
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	DEBUG("ENV:");
	for (auto &e : _env)
		DEBUG(e);
	if (pipe(_fds) == -1)
	{
		ERROR("Pipe failed");
		return;
	}
	if (write(_fds[1], request_packet.getContent().c_str(), request_packet.getContentSize()) == -1)
	{
		ERROR("Write failed");
		return;
	}
	(void)response_packet;
	execute();
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
		execve(_path_info.c_str(), argv, env.data());
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