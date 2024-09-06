#include "ServerConfig.hpp"
#include <regex>

//GETTERS

std::string ServerConfig::getHost() const { return _host; }

int ServerConfig::getPort() const { return _port; }

std::vector<std::string> ServerConfig::getServerNames() const { return _server_names; }

std::map<int, std::string> ServerConfig::getErrorPages() const { return error_pages; }

size_t ServerConfig::getClientMaxBodySize() const { return _client_max_body_size; }

std::map<std::string, RouteConfig> ServerConfig::getRoutes() const { return _routes; }

//SETTERS

void ServerConfig::setHost(const std::string& host) { _host = host; }

void ServerConfig::setPort(int port)
{
	if (port < 0 || port > 65535)
		throw std::invalid_argument("Invalid port: " + std::to_string(port));
	_port = port;
}

void ServerConfig::addServerName(const std::string& server_name)
{
	if (std::find(_server_names.begin(), _server_names.end(), server_name) != _server_names.end())
		return;
	_server_names.push_back(server_name);
}

void ServerConfig::addErrorPage(int error_code, const std::string& error_page)
{
	if (error_pages.find(error_code) != error_pages.end())
		return;
	error_pages[error_code] = error_page;
}

void ServerConfig::setClientMaxBodySize(int size)
{
	if (size < 0)
		throw std::invalid_argument("Invalid client max body size: " + std::to_string(size));
	_client_max_body_size = size;
}

void ServerConfig::addRoute(const std::string& route, const RouteConfig& config)
{
	if (_routes.find(route) != _routes.end())
		throw std::invalid_argument("Route already exists: " + route);
	if (route.empty())
		throw std::invalid_argument("Route cannot be empty");
	if (route[0] != '/')
		throw std::invalid_argument("Route must start with a forward slash: " + route);

	std::regex validRoutePattern("^/([a-zA-Z0-9_-]+/?)*$");
	if (!std::regex_match(route, validRoutePattern))
		throw std::invalid_argument("Route name contains invalid characters.");
	_routes[route] = config;
}
