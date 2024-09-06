#include "RouteConfig.hpp"

//GETTERS
std::string RouteConfig::getRoot() const { return _root; }

std::vector<std::string> RouteConfig::getAcceptedMethods() const { return _accepted_methods; }

std::string RouteConfig::getRedirectionUrl() const { return _redirection_url; }

bool RouteConfig::isAutoindex() const { return _autoindex; }

std::string RouteConfig::getDefaultFile() const { return _default_file; }

bool RouteConfig::isDirectoryListing() const { return _directory_listing;}

std::string RouteConfig::getUploadDirectory() const { return _upload_directory; }

//SETTERS
void RouteConfig::setRedirectionUrl(const std::string& url) { _redirection_url = url;}

void RouteConfig::setAutoindex(bool ai) { _autoindex = ai; }

void RouteConfig::setDefaultFile(const std::string& file) { _default_file = file; }

void RouteConfig::setUploadDirectory(const std::string& dir) { _upload_directory = dir; }

void RouteConfig::setRoot(const std::string& root) { _root = root; }

void RouteConfig::setDirectoryListing(bool dl) { _directory_listing = dl; }

void RouteConfig::setAcceptedMethods(const std::vector<std::string>& methods)
{
	for (const auto& method : methods)
	{
		if (std::find(_accepted_methods.begin(), _accepted_methods.end(), method) != _accepted_methods.end())
			return;
		if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT")
			throw std::invalid_argument("Invalid method: " + method);
		_accepted_methods.push_back(method);
	}
}
