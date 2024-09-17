#pragma once

#include "HttpPacket.hpp"
#include "ErrorResponse.hpp"
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"
#include "logging.hpp"

#include <vector>
#include <memory>

std::string handle_request(std::string request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs);
