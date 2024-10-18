#include "RequestHandler.hpp"

// TODO: check if the old response_packet has final response set -> also set it for the new response_packet

std::unique_ptr<ResponsePacket> handle_post(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
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
			bool final_response = response_packet.is_final_response();
			response_packet = std::make_unique<ResponsePacket>(ResponsePacket(cgi_response));
			response_packet.set_final_response(final_response);
			return response_packet;
		}
		catch (std::exception &e)
		{
			ERROR("CGI error: " + std::string(e.what()));
			return internal_server_error();
		}
	}
	else
	{
		// handle_upload(request_packet, response_packet, config_pair);
		// DEBUG("File request: " + uri_info.path);
		// std::optional<File> file = load_file_with_cache(uri_info.path);
		// if (!file)
		// 	return not_found(load_error_page(404, config_pair.first));
		// return ok_with_content(file.value(), std::move(response_packet));
	}
}

void handleUpload(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Handling upload request");
	std::string path = config_pair.second.getRoot() + request_packet.getUri();
	std::ofstream file(path, std::ios::binary);
	if (!file.is_open())
	{
		ERROR("Failed to open file: " + path);
		// return internal_server_error();
		return;
	}
	file << request_packet.getContent();
	file.close();
	DEBUG("File uploaded to: " + path);
	// return dummy_response();
	(void)response_packet;
}