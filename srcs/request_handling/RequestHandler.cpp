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
            handle_get(*request_packet, *response_packet, valid_config.value());
            break;
        case Method::POST:
            handle_post(*request_packet, *response_packet, valid_config.value());
            break;
        case Method::DELETE:
            handle_delete(*request_packet, *response_packet, valid_config.value());
            break;
        default:
            throw std::runtime_error("Unknown method");
        }
    }
    catch (std::exception &e)
    {
        DEBUG("Failed to handle request: " + std::string(e.what()));
        return internal_server_error();
    }

    return dummy_response();
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
        if (routes.find(packet.get_uri()) == routes.end())
        {
            it = configs.erase(it);
            continue;
        }

        // validate method
        auto accepted_methods = routes.at(packet.get_uri()).getAcceptedMethods();
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
        valid_config.second = configs[0].getRoutes().at(packet.get_uri());
        return valid_config;
    }
    else
        throw std::runtime_error("Multiple configurations found");
}

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