#pragma once

#include "HttpPacket.hpp"
#include "RequestHandlingUtils.hpp"

#include <string>
#include <memory>

std::unique_ptr<HttpPacket> internal_server_error();
std::unique_ptr<HttpPacket> bad_request();
std::unique_ptr<HttpPacket> dummy_response();
std::unique_ptr<HttpPacket> not_found();
std::unique_ptr<HttpPacket> ok_with_content(File &file, std::unique_ptr<HttpPacket> response_packet);
