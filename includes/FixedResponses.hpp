#pragma once

#include "ResponsePacket.hpp"
#include "RequestHandlingUtils.hpp"

#include <string>
#include <memory>

std::shared_ptr<ResponsePacket> internal_server_error();
std::shared_ptr<ResponsePacket> bad_request();
std::shared_ptr<ResponsePacket> dummy_response();
std::shared_ptr<ResponsePacket> not_found(const std::string &error_page);
std::shared_ptr<ResponsePacket> ok_with_content(File &file, std::shared_ptr<ResponsePacket> response_packet);
std::shared_ptr<ResponsePacket> redirect(const std::string &location);
std::shared_ptr<ResponsePacket> autoindex_response(std::string &index, std::shared_ptr<ResponsePacket> response_packet);
std::shared_ptr<ResponsePacket> payload_too_large();
std::shared_ptr<ResponsePacket> forbidden();
std::shared_ptr<ResponsePacket> created(const std::string &location);