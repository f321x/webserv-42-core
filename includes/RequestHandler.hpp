#pragma once

#include "RequestPacket.hpp"
#include "ResponsePacket.hpp"
#include "FixedResponses.hpp"
#include "ServerConfig.hpp"
#include "logging.hpp"
#include "RequestHandlingUtils.hpp"
#include "Cgi.hpp"

#include <vector>
#include <optional>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <iomanip>
#include <ctime>
#include <utility>
#include <sys/stat.h>
#include <thread>

std::shared_ptr<ResponsePacket> handle_request(RequestPacket &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs);
std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(RequestPacket &packet, const std::vector<ServerConfig> &available_configs);

// POST handling
std::shared_ptr<ResponsePacket> handle_post(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// GET handling
std::shared_ptr<ResponsePacket> handle_get(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
UriInfo getUri_info(const std::string &uri, const RouteConfig &route_config);
std::optional<File> load_file_with_cache(const std::string &filepath);
std::string load_error_page(int status_code, const ServerConfig &server);
std::string get_autoindex(std::string path, const std::string &uri);

// DELETE handling
std::shared_ptr<ResponsePacket> handle_delete(const RequestPacket &request_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
