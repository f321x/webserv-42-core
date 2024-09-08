#pragma once

#include "logging.hpp"
#include "SocketAddress.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include <string>
#include <stdexcept>
#include <map>
#include <stack>
#include <functional>
#include <fstream>

class WebServerConfig
{
public:
	WebServerConfig(const std::string &config_file_path); // real config parsed from path
	WebServerConfig();									  // dummy config for early testing
	~WebServerConfig();
	WebServerConfig(const WebServerConfig &other);
	WebServerConfig &operator=(const WebServerConfig &other);

	// should return the bind SocketAddress
	SocketAddress get_bind_address() const;
	ServerConfig getServerConfig() const;
	RouteConfig getRouteConfig(const std::string &route) const;

private:
	ServerConfig _server_config;
	std::string _bind_ip = "0.0.0.0"; // default value
	uint16_t _bind_port = 8080;		  // default value
};

// ServerConfig setters
void parseListen(ServerConfig &server, std::istringstream &stream);
void parseServerName(ServerConfig &server, std::istringstream &stream);
void parseErrorPage(ServerConfig &server, std::istringstream &stream);
void parseClientMaxBodySize(ServerConfig &server, std::istringstream &stream);
// RouteConfig setters
void parseRoot(RouteConfig &route, std::istringstream &stream);
void parseAcceptedMethods(RouteConfig &route, std::istringstream &stream);
void parseRedirection(RouteConfig &route, std::istringstream &stream);
void parseAutoindex(RouteConfig &route, std::istringstream &stream);
void parseDefaultFile(RouteConfig &route, std::istringstream &stream);
void parseDirectoryListing(RouteConfig &route, std::istringstream &stream);
void parseUploadDirectory(RouteConfig &route, std::istringstream &stream);