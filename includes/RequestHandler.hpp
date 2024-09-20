#pragma once

#include "HttpPacket.hpp"
#include "FixedResponses.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"
#include "logging.hpp"
#include "RequestHandlingUtils.hpp"

#include <vector>
#include <optional>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <iomanip>
#include <ctime>
#include <sys/stat.h>

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs);
std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(const HttpPacket &packet, const std::vector<ServerConfig> &available_configs);

// POST handling
void handle_post(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// GET handling
std::unique_ptr<HttpPacket> handle_get(const HttpPacket &request_packet, std::unique_ptr<HttpPacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
UriInfo get_uri_info(const std::string &uri, const RouteConfig &route_config);
std::optional<File> load_file_with_cache(const std::string &filepath);
std::string load_error_page(int status_code, const ServerConfig &server);
std::string get_autoindex(std::string path, const std::string &uri);

// DELETE handling
void handle_delete(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
