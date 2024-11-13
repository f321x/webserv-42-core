#include "RequestHandler.hpp"

std::unique_ptr<ResponsePacket> handle_delete(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    (void)request_packet;
    (void)response_packet;
    (void)config_pair;
    // check if uri ends with / or not
    // if it does not end with / check if a file exists, if not, return error. If a dir exists with the same name return 409
    // if it ends with / check if a dir exists, if not return error. If a file exists with the same name return 409
    // if write permission is denied return 403
    // if the file/dir exists, delete it return 204 / if deleting fails return 500
}
