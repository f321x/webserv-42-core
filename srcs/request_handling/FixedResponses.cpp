#include "ErrorResponse.hpp"

std::unique_ptr<HttpPacket> internal_server_error()
{
    auto response_packet = std::make_unique<HttpPacket>();
    response_packet->set_status_code(500);
    response_packet->set_status_message("Internal Server Error");
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<HttpPacket> bad_request()
{
    auto response_packet = std::make_unique<HttpPacket>();
    response_packet->set_status_code(400);
    response_packet->set_status_message("Bad Request");
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<HttpPacket> dummy_response()
{
    std::unique_ptr<HttpPacket> response_packet = std::make_unique<HttpPacket>();
    response_packet->set_status_code(200);
    response_packet->set_status_message("OK");
    response_packet->set_res_header("Content-Type", "text/html");
    response_packet->set_res_header("Content-Length", "38");
    response_packet->set_content("<html><body><h1>Hello, Browser!</h1></body></html>");
    return response_packet;
}