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
		handleUpload(request_packet, std::move(response_packet), config_pair);
		// DEBUG("File request: " + uri_info.path);
		// std::optional<File> file = load_file_with_cache(uri_info.path);
		// if (!file)
		// 	return not_found(load_error_page(404, config_pair.first));
		// return ok_with_content(file.value(), std::move(response_packet));
	}
	return response_packet;
}

bool handleUpload(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	// Parse the body to extract file content and metadata
	std::string file_name;
	std::string file_content;
	std::string body = request_packet.getContent();

	// Example: assuming body has already been processed and you know where to extract file info
	size_t file_start = body.find("\r\n\r\n") + 4;	   // Start of file content
	size_t file_end = body.find("------", file_start); // End of file content
	if (file_end == std::string::npos)
	{
		std::cerr << "Error: Unable to locate file content in request body." << std::endl;
		return false; // Return error if we can't parse the file content
	}

	file_content = body.substr(file_start, file_end - file_start);

	// For simplicity, we'll assume file_name is fixed for now
	file_name = "uploaded_file.txt"; // You should extract the filename from the Content-Disposition header

	// Construct the full file path
	std::string file_path = config_pair.second.getUploadDirectory() + "/" + file_name;

	// Write the file content to the upload directory
	std::ofstream outfile(file_path, std::ios::binary);
	if (!outfile.is_open())
	{
		std::cerr << "Error: Unable to open file for writing: " << file_path << std::endl;
		return false;
	}

	outfile << file_content;
	outfile.close();

	if (outfile.fail())
	{
		std::cerr << "Error: File writing failed for " << file_path << std::endl;
		return false;
	}

	std::cout << "File uploaded successfully: " << file_path << std::endl;
	(void)response_packet;
	return true;
}
