#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <optional>

#include "RequestPacket.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"

struct File
{
	std::string content;
	std::string file_ending;
};

enum UriType
{
	FILE_REQUEST,
	AUTOINDEX,
	REDIRECT, // redirect
	INVALID
};

struct UriInfo
{
	UriType type;
	std::string path;
};

bool check_keep_alive(const RequestPacket &packet);
std::string find_longest_matching_route(const std::string &uri, const std::map<std::string, RouteConfig> &routes);
std::string getContent_type(const std::string &file_ending);
bool has_file_ending(const std::string &uri);
std::string get_pure_hostname(RequestPacket &packet);
