#include "RequestHandler.hpp"

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    std::unique_ptr<HttpPacket> request_packet;
    std::unique_ptr<HttpPacket> response_packet;

    // Parse the request
    try
    {
        request_packet = std::make_unique<HttpPacket>(request);
    }
    catch (...)
    {
        DEBUG("Failed to parse request");
        return bad_request();
    }

    // Find the server config
    auto valid_config = find_valid_configuration(request_packet, available_configs);
    if (!valid_config.has_value())
    {
        return bad_request(); // use corect error type
    }

    if (!check_keep_alive(request_packet))
        response_packet->set_final_response();

    // dummy response
    return internal_server_error();
}

std::optional<std::unique_ptr<ServerConfig>> find_valid_configuration(const std::unique_ptr<HttpPacket> &packet, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    // for (const auto &server_config : *available_configs)
    // {
    //     for (const auto &route_config : server_config.get_route_configs())
    //     {
    //         if (route_config.get_path() == packet->get_uri())
    //         {
    //             return std::nullopt;
    //         }
    //     }
    // }
    return std::nullopt;
}

// https://datatracker.ietf.org/doc/html/rfc9112#section-9.3
bool check_keep_alive(const std::unique_ptr<HttpPacket> &packet)
{
    if (packet->get_req_header("Connection") == "keep-alive")
        return true;
    if (packet->get_http_version() == "HTTP/1.0")
        return false;
    if (packet->get_req_header("Connection") == "close")
        return false;

    return true;
}