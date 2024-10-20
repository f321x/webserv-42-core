#include "RequestHandler.hpp"

bool handleUpload(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);

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
			response_packet = std::make_unique<ResponsePacket>(ResponsePacket(cgi_response));
			if (response_packet->is_final_response())
				response_packet->set_final_response();
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
		if (config_pair.second.getUploadDirectory().empty())
		{
			ERROR("Location doesnt support uploads");
			return forbidden();
		}
		if (handleUpload(request_packet, std::move(response_packet), config_pair))
			return created(config_pair.second.getUploadDirectory());
		return internal_server_error();
	}
	return response_packet;
}

// TODO: do not copy last newline to the file, doesnt belong to the file content.
// TODO: check if the file already exists and return 409 conflict if it does

// TODO: parse the filename from the content-disposition header
std::string getFilename(const std::string &body)
{
	// Find the "Content-Disposition" line in the body
	size_t disposition_start = body.find("Content-Disposition:");
	if (disposition_start == std::string::npos)
	{
		ERROR("Content-Disposition not found.");
		return "";
	}

	// Find the end of the Content-Disposition line
	size_t disposition_end = body.find("\r\n", disposition_start);
	if (disposition_end == std::string::npos)
	{
		ERROR("End of Content-Disposition not found.");
		return "";
	}

	// Extract the Content-Disposition line
	std::string disposition_line = body.substr(disposition_start, disposition_end - disposition_start);

	// Locate 'filename="' in the Content-Disposition line
	std::string filename_token = "filename=\"";
	size_t filename_start = disposition_line.find(filename_token);
	if (filename_start == std::string::npos)
	{
		ERROR("Filename not found in Content-Disposition.");
		return "";
	}

	// Extract the actual filename (after filename=")
	filename_start += filename_token.length();
	size_t filename_end = disposition_line.find("\"", filename_start);
	if (filename_end == std::string::npos)
	{
		ERROR("End of filename not found.");
		return "";
	}

	// Return the extracted filename
	return disposition_line.substr(filename_start, filename_end - filename_start);
}

bool handleUpload(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	// Parse the body to extract file content and metadata
	std::string file_name;
	std::string file_content;
	std::string body = request_packet.getContent();

	// Example: assuming body has already been processed and you know where to extract file info
	size_t boundary_start = body.find(request_packet.getBoundary());
	if (boundary_start == std::string::npos)
	{
		ERROR("Multipart boundary not found in the request body.");
		return false;
	}

	// Skip to the content after the headers (i.e., after Content-Disposition and Content-Type)
	size_t headers_end = body.find("\r\n\r\n", boundary_start) + 4; // End of headers
	if (headers_end == std::string::npos)
	{
		ERROR("Failed to find the end of multipart headers.");
		return false;
	}

	// Find the end of the file content (before the next boundary)
	size_t file_end = body.find(request_packet.getBoundary(), headers_end) - 4; // Subtract 4 to exclude the trailing \r\n
	if (file_end == std::string::npos)
	{
		ERROR("Failed to locate the end of the file content.");
		return false;
	}
	DEBUG("File start: " + std::to_string(headers_end));
	DEBUG("File end: " + std::to_string(file_end));
	DEBUG(body);

	file_content = body.substr(headers_end, file_end - headers_end);

	// For simplicity, we'll assume file_name is fixed for now
	file_name = getFilename(request_packet.getContent());

	// Construct the full file path
	std::string file_path = config_pair.second.getUploadDirectory() + "/" + file_name;

	// Write the file content to the upload directory
	std::ofstream outfile(file_path, std::ios::binary);
	if (!outfile.is_open())
	{
		ERROR("Unable to open file for writing: " + file_path);
		return false;
	}

	outfile << file_content;
	outfile.close();

	if (outfile.fail())
	{
		ERROR("File writing failed for " + file_path);
		return false;
	}

	DEBUG("File uploaded successfully: " + file_path);
	(void)response_packet;
	return true;
}
