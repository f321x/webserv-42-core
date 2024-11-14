#include "FixedResponses.hpp"

std::shared_ptr<ResponsePacket> build_fixed_response(int status_code, const std::string &status_message)
{
	auto response_packet = std::make_shared<ResponsePacket>();
	response_packet->set_status_code(status_code);
	response_packet->set_status_message(status_message);
	response_packet->set_final_response();
	response_packet->setResponseReady(true);
	return response_packet;
}

std::shared_ptr<ResponsePacket> ok_with_content(File &file, std::shared_ptr<ResponsePacket> response_packet)
{
	response_packet->set_status_code(200);
	response_packet->set_status_message("OK");
	response_packet->setHeader("Content-Type", getContent_type(file.file_ending));
	response_packet->setContent(file.content);
	response_packet->setResponseReady(true);
	return response_packet;
}

std::shared_ptr<ResponsePacket> autoindex_response(std::string &index, std::shared_ptr<ResponsePacket> response_packet)
{
	response_packet->set_status_code(200);
	response_packet->set_status_message("OK");
	response_packet->setHeader("Content-Type", "text/html");
	response_packet->setContent(index);
	response_packet->setResponseReady(true);
	return response_packet;
}

std::shared_ptr<ResponsePacket> created()
{
	return build_fixed_response(201, "Created");
}

std::shared_ptr<ResponsePacket> no_content()
{
	return build_fixed_response(204, "No Content");
}

std::shared_ptr<ResponsePacket> redirect(const std::string &location)
{
	auto response_packet = std::make_shared<ResponsePacket>();
	response_packet->set_status_code(301);
	response_packet->set_status_message("Moved Permanently");
	response_packet->setHeader("Location", location);
	response_packet->setResponseReady(true);
	return response_packet;
}

std::shared_ptr<ResponsePacket> bad_request()
{
	return build_fixed_response(400, "Bad Request");
}

std::shared_ptr<ResponsePacket> not_found(const std::string &error_page)
{
	auto response_packet = build_fixed_response(404, "Not Found");
	response_packet->setContent(error_page);
	response_packet->setHeader("Content-Type", "text/html");
	return response_packet;
}

std::shared_ptr<ResponsePacket> forbidden()
{
	return build_fixed_response(403, "Forbidden");
}

std::shared_ptr<ResponsePacket> method_not_allowed()
{
	return build_fixed_response(405, "Method Not Allowed");
}

std::shared_ptr<ResponsePacket> conflict()
{
	return build_fixed_response(409, "Conflict");
}

std::shared_ptr<ResponsePacket> payload_too_large()
{
	return build_fixed_response(413, "Payload Too Large");
}

std::shared_ptr<ResponsePacket> internal_server_error()
{
	return build_fixed_response(500, "Internal Server Error");
}

std::shared_ptr<ResponsePacket> not_implemented()
{
	return build_fixed_response(501, "Not Implemented");
}
