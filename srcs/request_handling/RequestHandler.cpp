#include "RequestHandler.hpp"

std::unique_ptr<HttpPacket> handle_request(const std::string &request, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
    std::unique_ptr<HttpPacket> request_packet;
    std::unique_ptr<HttpPacket> response_packet = std::make_unique<HttpPacket>();

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
    auto valid_config = find_valid_configuration(*request_packet, *available_configs);
    if (!valid_config.has_value())
        return bad_request(); // use correct error type
    if (!check_keep_alive(*request_packet))
        response_packet->set_final_response();

    // Handle the request according to the requested method
    try
    {
        switch (request_packet->get_method())
        {
        case Method::GET:
            response_packet = handle_get(*request_packet, std::move(response_packet), valid_config.value());
            break;
        case Method::POST:
            handle_post(*request_packet, *response_packet, valid_config.value());
            break;
        case Method::DELETE:
            handle_delete(*request_packet, *response_packet, valid_config.value());
            break;
        default:
            return bad_request();
        }
    }
    catch (std::exception &e)
    {
        DEBUG("Failed to handle request: " + std::string(e.what()));
        return internal_server_error();
    }

    return response_packet;
}

std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(const HttpPacket &packet, const std::vector<ServerConfig> &available_configs)
{
    std::vector<ServerConfig> configs(available_configs.begin(), available_configs.end());
    std::pair<ServerConfig, RouteConfig> valid_config;

    for (auto it = configs.begin(); it != configs.end();)
    {
        // check against server_name
        auto server_names = it->getServerNames();
        if (std::find(server_names.begin(), server_names.end(), packet.getPureHostname()) == server_names.end())
        {
            it = configs.erase(it);
            continue;
        }

        // check packet against available routes
        std::map<std::string, RouteConfig> routes = it->getRoutes();
        std::string matching_route = find_longest_matching_route(packet.get_uri(), routes);
        if (matching_route.empty() || packet.get_uri().length() < matching_route.length())
        {
            it = configs.erase(it);
            continue;
        }

        // validate method
        auto accepted_methods = routes.at(matching_route).getAcceptedMethods();
        if (std::find(accepted_methods.begin(), accepted_methods.end(), packet.get_method()) == accepted_methods.end())
        {
            it = configs.erase(it);
            continue;
        }

        it++;
    }

    if (configs.size() == 0)
        return std::nullopt;
    else if (configs.size() == 1)
    {
        valid_config.first = configs[0];
        valid_config.second = configs[0].getRoutes().at(find_longest_matching_route(packet.get_uri(), configs[0].getRoutes()));
        return valid_config;
    }
    else
        throw std::runtime_error("Multiple configurations found");
}
