#pragma once

#include "HttpPacket.hpp"

#include <string>
#include <memory>

std::unique_ptr<HttpPacket> internal_server_error();
std::unique_ptr<HttpPacket> bad_request();
