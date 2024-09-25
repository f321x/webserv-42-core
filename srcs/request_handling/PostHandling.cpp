#include "RequestHandler.hpp"

void handle_post(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    DEBUG("Handling POST request");
    DEBUG("isCgi? " + std::string(1, config_pair.second.isCgi() ? '1' : '0'));
    DEBUG("Route: " + config_pair.second.getRoute());
    if (config_pair.second.isCgi())
    {
        Cgi(request_packet, response_packet, config_pair);
    }
    else
    {
        // handle_upload(request_packet, response_packet, config_pair);
    }
    (void)request_packet;
    (void)response_packet;
    // (void)config_pair;
}
