#pragma once

#include "logging.hpp"
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
	// SocketAddress get_bind_address() const;

	std::vector<ServerConfig> getServerConfigs() const;
	RouteConfig getRouteConfig(const std::string &route) const;

	// debug
	void printConfig() const;

private:
	std::vector<ServerConfig> _server_configs;
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
