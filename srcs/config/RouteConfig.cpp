#include "RouteConfig.hpp"
#include <iostream>

// GETTERS
std::string RouteConfig::getRoot() const { return _root; }

std::vector<Method> RouteConfig::getAcceptedMethods() const // added enum for easier comparison with HttpPacket
{
	std::vector<Method> accepted_methods;
	for (const std::string &method : _accepted_methods)
	{
		if (method == "GET")
			accepted_methods.push_back(GET);
		else if (method == "POST")
			accepted_methods.push_back(POST);
		else if (method == "DELETE")
			accepted_methods.push_back(DELETE);
	}
	return accepted_methods;
}

std::string RouteConfig::getRedirection() const { return _redirection_url; }

std::string RouteConfig::getRoute() const { return _route; }

bool RouteConfig::isAutoindex() const { return _autoindex; }

std::string RouteConfig::getDefaultFile() const { return _default_file; }

std::string RouteConfig::getUploadDirectory() const { return _upload_directory; }

std::string RouteConfig::getCgi(std::string extension) const
{
	try
	{
		return _cgi.at(extension);
	}
	catch (const std::out_of_range &e)
	{
		return "";
	}
}

std::unordered_map<std::string, std::string> RouteConfig::getCgi() const { return _cgi; }

// SETTERS
void RouteConfig::setRedirectionUrl(const std::string &url) { _redirection_url = url; }

void RouteConfig::setAutoindex(bool ai) { _autoindex = ai; }

void RouteConfig::setDefaultFile(const std::string &file) { _default_file = file; }

void RouteConfig::setUploadDirectory(const std::string &dir) { _upload_directory = dir; }

void RouteConfig::setRoot(const std::string &root) { _root = root; }

void RouteConfig::setRoute(const std::string &route) { _route = route; }

void RouteConfig::setAcceptedMethods(const std::vector<std::string> &methods)
{
	for (const auto &method : methods)
	{
		if (std::find(_accepted_methods.begin(), _accepted_methods.end(), method) != _accepted_methods.end())
			return;
		if (method != "GET" && method != "POST" && method != "DELETE")
			throw std::invalid_argument("Invalid method: " + method);
		_accepted_methods.push_back(method);
	}
}

void RouteConfig::addCgi(std::string extension, std::string path) { _cgi.emplace(extension, path); }

void RouteConfig::checkRouteConfig() const
{
	if (_root.empty())
		throw std::runtime_error("Root cannot be empty");
	if (_accepted_methods.empty())
		throw std::runtime_error("Accepted methods cannot be empty");
}