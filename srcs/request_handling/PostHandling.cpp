#include "RequestHandler.hpp"

void handle_post(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Handling POST request");
	DEBUG("isCgi? " + std::string(1, config_pair.second.isCgi() ? '1' : '0'));
	DEBUG("Route: " + config_pair.second.getRoute());
	if (config_pair.second.isCgi())
	{
		Cgi cgi(request_packet, config_pair);
		DEBUG("before execute");
		cgi.execute(request_packet);
		DEBUG("after execute");
		std::string cgi_response = cgi.getResponse();
		DEBUG("CGI response: " + cgi_response);
		auto tmp = ResponsePacket(cgi_response);
		response_packet = tmp;
	}
	else
	{
		// handle_upload(request_packet, response_packet, config_pair);
	}
}
