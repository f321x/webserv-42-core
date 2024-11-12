#include "Cgi.hpp"

#include <string>
#include <set>

std::shared_ptr<ResponsePacket> handleCgiRequest(const RequestPacket &request_packet, std::optional<std::pair<ServerConfig, RouteConfig>> &valid_config)
{
	std::shared_ptr<ResponsePacket> response;

	try
	{
		auto cgi = Cgi(request_packet, valid_config.value());
		cgi.execute(request_packet);
		auto cgi_response = cgi.getResponse();
		response = std::make_shared<ResponsePacket>(cgi_response);
		if (!check_keep_alive(request_packet))
			response->set_final_response();
	}
	catch (std::exception &e)
	{
		DEBUG("CGI error: " + std::string(e.what()));
		response = internal_server_error();
	}
	// response_packet->setResponseReady(true);
	return response;
}