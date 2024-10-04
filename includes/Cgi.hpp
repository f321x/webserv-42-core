#pragma once

#include <string>
#include <utility>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>
#include "RequestPacket.hpp"
#include "ResponsePacket.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"

class Cgi
{
private:
	std::vector<std::string> _env;
	std::string _path_info;
	int _fds[2];
	std::string _cgi_response;
	void writeToPipe(const std::string &content, int fd);
	std::string readFromPipe(int fd);

public:
	Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
	void execute();
	std::string getResponse() const;
};