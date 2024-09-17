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
