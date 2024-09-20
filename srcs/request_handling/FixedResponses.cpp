#include "FixedResponses.hpp"

std::unique_ptr<ResponsePacket> internal_server_error()
{
    auto response_packet = std::make_unique<ResponsePacket>();
    response_packet->set_status_code(500);
    response_packet->set_status_message("Internal Server Error");
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<ResponsePacket> bad_request()
{
    auto response_packet = std::make_unique<ResponsePacket>();
    response_packet->set_status_code(400);
    response_packet->set_status_message("Bad Request");
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<ResponsePacket> dummy_response()
{
    std::unique_ptr<ResponsePacket> response_packet = std::make_unique<ResponsePacket>();
    response_packet->set_status_code(200);
    response_packet->set_status_message("OK");
    response_packet->set_res_header("Content-Type", "text/html");
    response_packet->set_res_header("Content-Length", "38");
    response_packet->set_content("<html><body><h1>Hello, Browser!</h1></body></html>");
    return response_packet;
}

std::unique_ptr<ResponsePacket> not_found(const std::string &error_page)
{
    auto response_packet = std::make_unique<ResponsePacket>();
    response_packet->set_status_code(404);
    response_packet->set_status_message("Not found");
    response_packet->set_content(error_page);
    response_packet->set_res_header("Content-Type", "text/html");
    response_packet->set_res_header("Content-Length", std::to_string(error_page.size()));
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<ResponsePacket> ok_with_content(File &file, std::unique_ptr<ResponsePacket> response_packet)
{
    response_packet->set_status_code(200);
    response_packet->set_status_message("OK");
    response_packet->set_res_header("Content-Type", get_content_type(file.file_ending));
    response_packet->set_res_header("Content-Length", std::to_string(file.content.size()));
    response_packet->set_content(file.content);
    return response_packet;
}

std::unique_ptr<ResponsePacket> redirect(const std::string &location)
{
    auto response_packet = std::make_unique<ResponsePacket>();
    response_packet->set_status_code(301);
    response_packet->set_status_message("Moved Permanently");
    response_packet->set_res_header("Location", location);
    response_packet->set_final_response();
    return response_packet;
}

std::unique_ptr<ResponsePacket> autoindex_response(std::string &index, std::unique_ptr<ResponsePacket> response_packet)
{
    response_packet->set_status_code(200);
    response_packet->set_status_message("OK");
    response_packet->set_res_header("Content-Type", "text/html");
    response_packet->set_res_header("Content-Length", std::to_string(index.size()));
    response_packet->set_content(index);
    return response_packet;
}
