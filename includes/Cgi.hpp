#pragma once

#include <string>
#include <utility>
#include "RequestPacket.hpp"
#include "ResponsePacket.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"

class Cgi
{
private:
	std::string _script_name;
	std::string _path_info;
	std::string _query_string;
	int _fds[2];
	int _pid;

public:
	Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
	// execute();
};