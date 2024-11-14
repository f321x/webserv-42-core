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
		DEBUG("CGI request handled");
	}
	catch (Cgi::TimedOutException &e)
	{
		ERROR("CGI timed out");
		*response = *build_fixed_response(504, "Gateway Timeout");
	}
	catch (std::exception &e)
	{
		DEBUG("CGI error: " + std::string(e.what()));
		*response = *internal_server_error();
	}
	if (!check_keep_alive(request_packet))
		response->set_final_response();
	response->setResponseReady(true);
	TRACE("Got CGI response");
}
