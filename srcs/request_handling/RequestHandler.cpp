#include "RequestHandler.hpp"

// TODO: make something that works with multiple threads and with the ->has_value() check in the main loop

std::shared_ptr<ResponsePacket> handle_request(RequestPacket &request_packet, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
	// Find the server config
	auto valid_config = find_valid_configuration(request_packet, *available_configs);
	if (!valid_config.has_value())
		return bad_request(); // use correct error type | (niklas) did also return 400 when the method wasnt allowed -> 405?
	DEBUG("Valid config found");
	// TODO: check if the request is a valid cgi request (extension matches with a cgi path)
	DEBUG("weirde cgi check:" + std::to_string(request_packet.getUri().find_last_of('.')));
	size_t pos = request_packet.getUri().find_last_of('.');
	if (pos != std::string::npos && !valid_config->second.getCgi(request_packet.getUri().substr(pos)).empty() && (request_packet.getMethod() == Method::POST || request_packet.getMethod() == Method::GET))
	{
		DEBUG("CGI happening");
		// std::thread cgi_thread(handleCgiRequest, std::ref(*request_packet), response_packet, std::ref(valid_config));
		// cgi_thread.detach();
		return handleCgiRequest(request_packet, valid_config);
		// return response_packet;
	}

	std::shared_ptr<ResponsePacket> response_packet = std::make_shared<ResponsePacket>();
	if (!check_keep_alive(request_packet))
		response_packet->set_final_response();
	// Handle the request according to the requested method
	try
	{
		switch (request_packet.getMethod())
		{
		case Method::GET:
			response_packet = handle_get(request_packet, response_packet, valid_config.value());
			break;
		case Method::POST:
			response_packet = handle_post(request_packet, response_packet, valid_config.value());
			break;
		case Method::DELETE:
			handle_delete(request_packet, *response_packet, valid_config.value());
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
	DEBUG("Request handled");
	// response_packet->setResponseReady(true);
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
		if (std::find(server_names.begin(), server_names.end(), get_pure_hostname(packet)) == server_names.end())
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
		return std::nullopt;
	else if (configs.size() == 1)
	{
		valid_config.first = configs[0];
		valid_config.second = configs[0].getRoutes().at(find_longest_matching_route(packet.getUri(), configs[0].getRoutes()));
		return valid_config;
	}
	else
		throw std::runtime_error("Multiple configurations found");
}
