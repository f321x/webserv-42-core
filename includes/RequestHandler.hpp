#pragma once

#include "HttpPacket.hpp"
#include "ErrorResponse.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"
#include "logging.hpp"

#include <vector>
#include <optional>
#include <memory>

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs);
bool check_keep_alive(const HttpPacket &packet);
std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(const HttpPacket &packet, const std::vector<ServerConfig> &available_configs);
std::unique_ptr<HttpPacket> dummy_response();
std::string find_longest_matching_route(const std::string &uri, const std::map<std::string, RouteConfig> &routes);

// POST handling
void handle_post(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// GET handling
void handle_get(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

// DELETE handling
void handle_delete(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);