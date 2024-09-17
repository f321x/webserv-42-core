#include "RequestHandler.hpp"

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    // Parse the request
    std::unique_ptr<HttpPacket> packet;
    try
    {
        packet = std::make_unique<HttpPacket>(request);
    }
    catch (...)
    {
        DEBUG("Failed to parse request");
        return bad_request();
    }

    // Find the server config
    auto invalid_config_packet = validate_against_config(packet, available_configs);
    // dummy response
    return internal_server_error();
}

std::optional<std::unique_ptr<HttpPacket>> validate_against_config(const std::unique_ptr<HttpPacket> &packet, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    return std::nullopt;
}
