#include "Cgi.hpp"

#include <string>
#include <set>

std::shared_ptr<ResponsePacket> handleCgiRequest(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, std::optional<std::pair<ServerConfig, RouteConfig>> &valid_config)
{
	try
	{
		auto cgi = Cgi(request_packet, valid_config.value());
		cgi.execute(request_packet);
		auto cgi_response = cgi.getResponse();
		*response_packet = ResponsePacket(cgi_response);
	}
	catch (std::exception &e)
	{
		DEBUG("CGI error: " + std::string(e.what()));
		*response_packet = *internal_server_error();
	}
	return response_packet;
}

bool validCgiFileEnding(const std::string &path)
{
	// List of allowed CGI file extensions
	static const std::set<std::string> allowed_extensions = {".pl", ".php", ".py"};

	// Find the last dot in the path to extract the file extension
	size_t dot_position = path.find_last_of('.');
	if (dot_position == std::string::npos)
		return false; // No extension found

	// Extract the file extension from the path
	std::string extension = path.substr(dot_position);

	// Check if the extension is in the set of allowed extensions
	return allowed_extensions.find(extension) != allowed_extensions.end();
}
