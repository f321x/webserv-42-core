#include "RequestHandler.hpp"

// https://datatracker.ietf.org/doc/html/rfc9112#section-9.3
bool check_keep_alive(const HttpPacket &packet)
{
    if (packet.get_req_header("Connection") == "keep-alive")
        return true;
    if (packet.get_http_version() == "HTTP/1.0")
        return false;
    if (packet.get_req_header("Connection") == "close")
        return false;

    return true;
}

std::string find_longest_matching_route(const std::string &uri, const std::map<std::string, RouteConfig> &routes)
{
    std::string longest_matching_route;
    for (const auto &route : routes)
    {
        if (uri.compare(0, route.first.length(), route.first) == 0)
        {
            if (route.first.length() > longest_matching_route.length())
                longest_matching_route = route.first;
        }
    }
    return longest_matching_route;
}
