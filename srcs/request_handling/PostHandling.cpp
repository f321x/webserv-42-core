#include "RequestHandler.hpp"

bool handleUpload(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair);
std::string getFilename(const std::string &headers);
bool writeFile(const std::string &file_path, const std::string &file_content);

// TODO: check if the old response_packet has final response set -> also set it for the new response_packet

std::shared_ptr<ResponsePacket> handle_post(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	DEBUG("Handling POST request");
	DEBUG("Route: " + config_pair.second.getRoute());
	if (config_pair.second.getUploadDirectory().empty())
	{
		ERROR("Location doesnt support uploads");
		return forbidden();
	}
	if (handleUpload(request_packet, response_packet, config_pair))
		return created(config_pair.second.getUploadDirectory());
	return internal_server_error();
	return response_packet;
}

// Corrected handleUpload function
bool handleUpload(const RequestPacket &request_packet, std::shared_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	std::string body = request_packet.getContent();
	std::string boundary = request_packet.getBoundary();
	std::string boundary_marker = "--" + boundary;
	size_t pos = 0;
	size_t boundary_start = body.find(boundary_marker, pos);
	if (boundary_start == std::string::npos)
		return ERROR("Multipart boundary not found in the request body."), false;
	pos = boundary_start + boundary_marker.length();

	// Expect a CRLF after the boundary marker
	if (body.substr(pos, 2) != "\r\n")
		return ERROR("Expected CRLF after the boundary marker."), false;
	pos += 2; // Move past the "\r\n"

	size_t headers_end = body.find("\r\n\r\n", pos);
	if (headers_end == std::string::npos)
		return ERROR("Failed to find the end of multipart headers."), false;
	std::string headers = body.substr(pos, headers_end - pos);
	std::string file_name;
	try
	{
		file_name = getFilename(headers);
	}
	catch (std::runtime_error &e)
	{
		return ERROR("Failed to extract filename from headers."), false;
	}
	size_t content_start = headers_end + 4; // Skip past "\r\n\r\n"
	std::string end_boundary = "\r\n" + boundary_marker;
	size_t boundary_end = body.find(end_boundary, content_start);
	if (boundary_end == std::string::npos)
	{
		end_boundary = "\r\n" + boundary_marker + "--";
		boundary_end = body.find(end_boundary, content_start);
		if (boundary_end == std::string::npos)
			return ERROR("Failed to find the ending boundary marker."), false;
	}
	std::string file_content = body.substr(content_start, boundary_end - content_start);
	std::string file_path = config_pair.second.getUploadDirectory() + "/" + file_name;
	if (!writeFile(file_path, file_content))
		return ERROR("File writing failed for " + file_path), false;

	DEBUG("File uploaded successfully: " + file_path);
	(void)response_packet;
	return true;
}

std::string getFilename(const std::string &headers)
{
	// Find the "Content-Disposition" line in the headers
	size_t disposition_start = headers.find("Content-Disposition:");
	if (disposition_start == std::string::npos)
		throw std::runtime_error("Content-Disposition not found in headers.");

	// Extract the Content-Disposition line
	size_t disposition_end = headers.find("\r\n", disposition_start);
	if (disposition_end == std::string::npos)
		disposition_end = headers.length();
	std::string disposition_line = headers.substr(disposition_start, disposition_end - disposition_start);

	// Locate 'filename="' in the Content-Disposition line
	std::string filename_token = "filename=\"";
	size_t filename_start = disposition_line.find(filename_token);
	if (filename_start == std::string::npos)
		throw std::runtime_error("Filename not found in Content-Disposition.");

	// Extract the actual filename (after filename=")
	filename_start += filename_token.length();
	size_t filename_end = disposition_line.find("\"", filename_start);
	if (filename_end == std::string::npos)
		throw std::runtime_error("End of filename not found.");

	// Return the extracted filename
	return disposition_line.substr(filename_start, filename_end - filename_start);
}

bool writeFile(const std::string &file_path, const std::string &file_content)
{
	std::ofstream outfile(file_path, std::ios::binary);
	if (!outfile.is_open())
		return ERROR("Unable to open file for writing: " + file_path), false;
	outfile.write(file_content.c_str(), file_content.size());
	outfile.close();
	if (outfile.fail())
		return ERROR("Failed to write to the file: " + file_path), false;
	return true;
}
