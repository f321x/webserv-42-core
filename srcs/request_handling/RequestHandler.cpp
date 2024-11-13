#include "RequestHandler.hpp"

std::unique_ptr<ResponsePacket> handle_request(RequestPacket &request_packet, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
	std::unique_ptr<ResponsePacket> response_packet = std::make_unique<ResponsePacket>();

	// Parse the request
	DEBUG("Request parsed");
	// Find the server config
	auto valid_config = find_valid_configuration(request_packet, *available_configs);
	if (!valid_config.has_value())
		return bad_request(); // use correct error type | (niklas) did also return 400 when the method wasnt allowed -> 405?
	if (!check_keep_alive(request_packet))
		response_packet->set_final_response();
	DEBUG("Valid config found");
	// Handle the request according to the requested method
	try
	{
		switch (request_packet.getMethod())
		{
		case Method::GET:
			response_packet = handle_get(request_packet, std::move(response_packet), valid_config.value());
			break;
		case Method::POST:
			response_packet = handle_post(request_packet, std::move(response_packet), valid_config.value());
			break;
		case Method::DELETE:
			handle_delete(request_packet, *response_packet, valid_config.value());
			break;
		}
	}
	catch (std::exception &e)
	{
		DEBUG("Failed to handle request: " + std::string(e.what()));
		return internal_server_error();
	}
	DEBUG("Request handled");
	return response_packet;
}

std::optional<std::pair<ServerConfig, RouteConfig>> find_valid_configuration(RequestPacket &packet, const std::vector<ServerConfig> &available_configs)
{
	std::vector<ServerConfig> configs(available_configs.begin(), available_configs.end());
	std::pair<ServerConfig, RouteConfig> valid_config;

	for (auto it = configs.begin(); it != configs.end();)
	{
		// check against server_name
		auto server_names = it->getServerNames();
		if (std::find(server_names.begin(), server_names.end(), getPureHostname(packet)) == server_names.end())
		{
			it = configs.erase(it);
			continue;
		}

		// check packet against available routes
		std::map<std::string, RouteConfig> routes = it->getRoutes();
		std::string matching_route = find_longest_matching_route(packet.getUri(), routes);
		if (matching_route.empty() || packet.getUri().length() < matching_route.length())
		{
			it = configs.erase(it);
			continue;
		}

		// validate method
		auto accepted_methods = routes.at(matching_route).getAcceptedMethods();
		if (std::find(accepted_methods.begin(), accepted_methods.end(), packet.getMethod()) == accepted_methods.end())
		{
			it = configs.erase(it);
			continue;
		}

		it++;
	}

	if (configs.size() == 0)
	{
		valid_config.first = available_configs[0];
		valid_config.second = available_configs[0].getRoutes().at(find_longest_matching_route(packet.getUri(), available_configs[0].getRoutes()));
		return valid_config;
	}
	else if (configs.size() == 1)
	{
		valid_config.first = configs[0];
		valid_config.second = configs[0].getRoutes().at(find_longest_matching_route(packet.getUri(), configs[0].getRoutes()));
		return valid_config;
	}
	else
		throw std::runtime_error("Multiple configurations found");
}
