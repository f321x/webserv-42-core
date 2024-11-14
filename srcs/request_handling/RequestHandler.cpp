#include "RequestHandler.hpp"

// TODO: make something that works with multiple threads and with the ->has_value() check in the main loop

std::shared_ptr<ResponsePacket> handle_request(RequestPacket &request_packet, const std::shared_ptr<std::vector<ServerConfig>> &available_configs)
{
	// Find the server config
	auto valid_config = find_valid_configuration(request_packet, *available_configs);
	if (!valid_config.has_value())
	{
		TRACE("No valid config found");
		return bad_request(); // use correct error type | (niklas) did also return 400 when the method wasnt allowed -> 405?
	}

	if (request_packet.isMethodNotAllowed())
	{
		TRACE("Method not allowed");
		return method_not_allowed();
	}

	auto response = std::make_shared<ResponsePacket>();
	TRACE("Valid config found");
	// TODO: check if the request is a valid cgi request (extension matches with a cgi path)
	TRACE("weirde cgi check:" + std::to_string(request_packet.getUri().find_last_of('.')));
	size_t pos = request_packet.getUri().find_last_of('.');
	if (pos != std::string::npos && !valid_config->second.getCgi(request_packet.getUri().substr(pos)).empty() && (request_packet.getMethod() == Method::POST || request_packet.getMethod() == Method::GET))
	{
		TRACE("CGI handling started");
		std::thread cgi_thread(handleCgiRequest, std::move(request_packet), std::move(valid_config.value()), response);
		cgi_thread.detach();
		// return handleCgiRequest(request_packet, valid_config);
		return response;
	}

	// std::shared_ptr<ResponsePacket> response = std::make_shared<ResponsePacket>();
	if (!check_keep_alive(request_packet))
		response->set_final_response();
	// response->setResponseReady(true);
	// Handle the request according to the requested method
	try
	{
		switch (request_packet.getMethod())
		{
		case Method::GET:
			response = handle_get(request_packet, response, valid_config.value());
			break;
		case Method::POST:
			response = handle_post(request_packet, response, valid_config.value());
			break;
		case Method::DELETE:
			response = handle_delete(request_packet, valid_config.value());
			break;
		}
	}
	catch (std::exception &e)
	{
		TRACE("Failed to handle request: " + std::string(e.what()));
		return internal_server_error();
	}
	TRACE("Request handled");
	response->setResponseReady(true);
	return response;
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

		it++;
	}

	if (configs.size() == 0)
	{
		valid_config.first = available_configs[0];
		valid_config.second = available_configs[0].getRoutes().at(find_longest_matching_route(packet.getUri(), available_configs[0].getRoutes()));
	}
	else if (configs.size() == 1)
	{
		valid_config.first = configs[0];
		valid_config.second = configs[0].getRoutes().at(find_longest_matching_route(packet.getUri(), configs[0].getRoutes()));
	}
	else
		throw std::runtime_error("Multiple configurations found");

	// Check if method is allowed
	auto accepted_methods = valid_config.second.getAcceptedMethods();
	if (std::find(accepted_methods.begin(), accepted_methods.end(), packet.getMethod()) == accepted_methods.end())
		packet.setMethodNotAllowed(true);

	return valid_config;
}
