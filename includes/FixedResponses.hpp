#pragma once

#include "ResponsePacket.hpp"
#include "RequestHandlingUtils.hpp"

#include <string>
#include <memory>

// Helper function
std::shared_ptr<ResponsePacket> build_fixed_response(int status_code, const std::string &status_message);

// 2xx Success                                                                    // 200
std::shared_ptr<ResponsePacket> ok_with_content(File &file, std::shared_ptr<ResponsePacket> response_packet);            // 200
std::shared_ptr<ResponsePacket> autoindex_response(std::string &index, std::shared_ptr<ResponsePacket> response_packet); // 200
std::shared_ptr<ResponsePacket> created();                                                                               // 201
std::shared_ptr<ResponsePacket> no_content();                                                                            // 204

// 3xx Redirection
std::shared_ptr<ResponsePacket> redirect(const std::string &location); // 301

// 4xx Client Errors
std::shared_ptr<ResponsePacket> bad_request();                            // 400
std::shared_ptr<ResponsePacket> forbidden();                              // 403
std::shared_ptr<ResponsePacket> not_found(const std::string &error_page); // 404
std::shared_ptr<ResponsePacket> method_not_allowed();                     // 405
std::shared_ptr<ResponsePacket> conflict();                               // 409
std::shared_ptr<ResponsePacket> payload_too_large();                      // 413

// 5xx Server Errors
std::shared_ptr<ResponsePacket> internal_server_error(); // 500
std::shared_ptr<ResponsePacket> not_implemented();       // 501
