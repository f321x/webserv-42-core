#include "RequestHandler.hpp"

void handle_post(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Handling POST request");
	DEBUG("isCgi? " + std::string(1, config_pair.second.isCgi() ? '1' : '0'));
	DEBUG("Route: " + config_pair.second.getRoute());
	if (validCgiFileEnding(request_packet.getUri()) && config_pair.second.isCgi())
	{
		try
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
		catch (std::exception &e)
		{
			ERROR("CGI error: " + std::string(e.what()));
			// return internal_server_error();
			return;
		}
	}
	else
	{
		// handle_upload(request_packet, response_packet, config_pair);
		DEBUG("File request: " + uri_info.path);
		// std::optional<File> file = load_file_with_cache(uri_info.path);
		// if (!file)
		// 	return not_found(load_error_page(404, config_pair.first));
		// return ok_with_content(file.value(), std::move(response_packet));
	}
}
