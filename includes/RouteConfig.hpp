#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

class RouteConfig
{
private:
	std::string _root;
	std::vector<std::string> _accepted_methods;
	std::string _redirection_url;
	bool _autoindex;
	std::string _default_file;
	bool _directory_listing;
	std::string _upload_directory;
public:
	//CONSTRUCTOR
	RouteConfig() : _autoindex(false), _directory_listing(false) {}

	//GETTERS
	std::string getRoot() const;
	std::vector<std::string> getAcceptedMethods() const;
	std::string getRedirection() const;
	bool isAutoindex() const;
	std::string getDefaultFile() const;
	bool isDirectoryListing() const;
	std::string getUploadDirectory() const;

	//SETTERS
	void setRedirectionUrl(const std::string& url);
	void setAutoindex(bool ai);
	void setDefaultFile(const std::string& file);
	void setUploadDirectory(const std::string& dir);
	void setRoot(const std::string& root);
	void setDirectoryListing(bool dl);
	void setAcceptedMethods(const std::vector<std::string>& methods);
};