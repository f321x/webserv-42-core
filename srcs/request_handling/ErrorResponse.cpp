#include "ErrorResponse.hpp"

std::string internal_server_error()
{
    auto response_packet = HttpPacket();
    response_packet.set_status_code(500);
    response_packet.set_status_message("Internal Server Error");
    return response_packet.serializeResponse();
}

std::string bad_request()
{
    auto response_packet = HttpPacket();
    response_packet.set_status_code(400);
    response_packet.set_status_message("Bad Request");
    return response_packet.serializeResponse();
}