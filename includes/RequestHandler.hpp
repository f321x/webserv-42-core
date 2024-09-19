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

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs);
std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(const HttpPacket &packet, const std::vector<ServerConfig> &available_configs);
std::optional<File> load_file_with_cache(const std::string &filepath);

// POST handling
void handle_post(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// GET handling
std::unique_ptr<HttpPacket> handle_get(const HttpPacket &request_packet, std::unique_ptr<HttpPacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// DELETE handling
void handle_delete(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
