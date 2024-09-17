#include "RequestHandler.hpp"

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    (void)available_configs;
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

    // dummy response
    return internal_server_error();
}
