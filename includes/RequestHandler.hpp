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
std::optional<std::unique_ptr<ServerConfig>> find_valid_configuration(const HttpPacket &packet, const std::vector<ServerConfig> &available_configs);
std::unique_ptr<HttpPacket> dummy_response();