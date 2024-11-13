#pragma once

#include "ResponsePacket.hpp"
#include "RequestHandlingUtils.hpp"

#include <string>
#include <memory>

std::unique_ptr<ResponsePacket> internal_server_error();
std::unique_ptr<ResponsePacket> bad_request();
std::unique_ptr<ResponsePacket> dummy_response();
std::unique_ptr<ResponsePacket> not_found(const std::string &error_page);
std::unique_ptr<ResponsePacket> ok_with_content(File &file, std::unique_ptr<ResponsePacket> response_packet);
std::unique_ptr<ResponsePacket> redirect(const std::string &location);
std::unique_ptr<ResponsePacket> autoindex_response(std::string &index, std::unique_ptr<ResponsePacket> response_packet);
std::unique_ptr<ResponsePacket> payload_too_large();
std::unique_ptr<ResponsePacket> forbidden();
std::unique_ptr<ResponsePacket> created(const std::string &location);
std::unique_ptr<ResponsePacket> not_implemented();