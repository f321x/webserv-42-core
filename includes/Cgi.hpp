#pragma once

#include <string>
#include <utility>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <optional> // For std::optional
#include <memory>
#include <signal.h>

#include "RequestPacket.hpp"
#include "ResponsePacket.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"
#include "logging.hpp"
#include "FixedResponses.hpp"

class Cgi
{
private:
	std::vector<std::string> _env;
	std::string _path_info;
	std::string _cgi_response;
	pid_t _pid;
	int _input_pipe[2];
	int _output_pipe[2];

	void writeToPipe(const std::string &content, int fd);
	std::string readFromPipe(int fd);

public:
	Cgi(const RequestPacket &request_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
	~Cgi();
	std::string execute(const RequestPacket &request_packet);
	std::string getResponse() const;
	class TimedOutException : public std::exception
	{
		const char *what() const noexcept override
		{
			return "Cgi execution timed out";
		}
	};
};

void handleCgiRequest(const RequestPacket request_packet, const std::pair<ServerConfig, RouteConfig> valid_config, std::shared_ptr<ResponsePacket> response);