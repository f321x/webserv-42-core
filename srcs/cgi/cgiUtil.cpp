#include "Cgi.hpp"

#include <string>
#include <set>

void handleCgiRequest(
	const RequestPacket request_packet,
	const std::pair<ServerConfig, RouteConfig> valid_config,
	std::shared_ptr<ResponsePacket> response)
{
	try
	{
		Cgi cgi(request_packet, valid_config);
		auto cgi_response = cgi.execute(request_packet);
		// auto cgi_response = cgi.getResponse();
		response->constructCgiResponse(cgi_response);
		if (!check_keep_alive(request_packet))
			response->set_final_response();
		response->setResponseReady(true);
		DEBUG("CGI request handled");
	}
	catch (std::exception &e)
	{
		DEBUG("CGI error: " + std::string(e.what()));
		response = internal_server_error();
	}
	TRACE("Got CGI response");
}
