#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include "Utils.hpp" // for Method enum

class RouteConfig
{
private:
	std::string _root;
	std::string _route;
	std::string _redirection_url;
	std::string _default_file;
	std::string _upload_directory;
	std::vector<std::string> _accepted_methods;
	std::unordered_map<std::string, std::string> _cgi; // key: extension, value: path
	bool _autoindex;

public:
	// CONSTRUCTOR
	RouteConfig() : _autoindex(false) {}

	// GETTERS
	std::string getRoot() const;
	std::string getRoute() const;
	std::vector<Method> getAcceptedMethods() const;
	std::string getRedirection() const;
	bool isAutoindex() const;
	std::string getDefaultFile() const;
	std::string getUploadDirectory() const;
	std::string getCgi(std::string extension) const;
	std::unordered_map<std::string, std::string> getCgi() const;

	// SETTERS
	void setRedirectionUrl(const std::string &url);
	void setAutoindex(bool ai);
	void setDefaultFile(const std::string &file);
	void setUploadDirectory(const std::string &dir);
	void setRoot(const std::string &root);
	void setRoute(const std::string &route);
	void setDirectoryListing(bool dl);
	void setAcceptedMethods(const std::vector<std::string> &methods);
	void addCgi(std::string extension, std::string path);

	// CHECKER
	void checkRouteConfig() const;
};