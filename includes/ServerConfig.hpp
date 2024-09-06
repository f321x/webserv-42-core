#pragma once

#include <string>
#include <vector>
#include <map>
#include "RouteConfig.hpp"

class ServerConfig
{
private:
	std::string _host;
	int _port;
	std::vector<std::string> _server_names;
	std::map<int, std::string> error_pages;
	size_t _client_max_body_size;
	std::map<std::string, RouteConfig> _routes;

public:
	ServerConfig() : _port(80), _client_max_body_size(1048576) {};

	//GETTERS
	std::string getHost() const;
	int getPort() const;
	std::vector<std::string> getServerNames() const;
	std::map<int, std::string> getErrorPages() const;
	size_t getClientMaxBodySize() const;
	std::map<std::string, RouteConfig> getRoutes() const;

	//SETTERS
	void setHost(const std::string& host);
	void setPort(int port);
	void addServerName(const std::string& server_name);
	void addErrorPage(int error_code, const std::string& error_page);
	void setClientMaxBodySize(int size);
	void addRoute(const std::string& route, const RouteConfig& config);
};