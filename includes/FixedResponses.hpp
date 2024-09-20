#pragma once

#include "HttpPacket.hpp"
#include "RequestHandlingUtils.hpp"

#include <string>
#include <memory>

std::unique_ptr<HttpPacket> internal_server_error();
std::unique_ptr<HttpPacket> bad_request();
std::unique_ptr<HttpPacket> dummy_response();
std::unique_ptr<HttpPacket> not_found(const std::string &error_page);
std::unique_ptr<HttpPacket> ok_with_content(File &file, std::unique_ptr<HttpPacket> response_packet);
std::unique_ptr<HttpPacket> redirect(const std::string &location);
std::unique_ptr<HttpPacket> autoindex_response(std::string &index, std::unique_ptr<HttpPacket> response_packet);