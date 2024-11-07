#include "FixedResponses.hpp"

std::shared_ptr<ResponsePacket> internal_server_error()
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(500);
	response_packet->set_status_message("Internal Server Error");
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> bad_request()
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(400);
	response_packet->set_status_message("Bad Request");
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> payload_too_large()
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(413);
	response_packet->set_status_message("Payload Too Large");
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> dummy_response()
{
	std::shared_ptr<ResponsePacket> response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(200);
	response_packet->set_status_message("OK");
	response_packet->setHeader("Content-Type", "text/html");
	response_packet->setContent("<html><body><h1>Hello, Browser!</h1></body></html>");
	return response_packet;
}

std::shared_ptr<ResponsePacket> not_found(const std::string &error_page)
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(404);
	response_packet->set_status_message("Not found");
	response_packet->setContent(error_page);
	response_packet->setHeader("Content-Type", "text/html");
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> ok_with_content(File &file, std::shared_ptr<ResponsePacket> response_packet)
{
	response_packet->set_status_code(200);
	response_packet->set_status_message("OK");
	response_packet->setHeader("Content-Type", getContent_type(file.file_ending));
	response_packet->setContent(file.content);
	return response_packet;
}

std::shared_ptr<ResponsePacket> redirect(const std::string &location)
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(301);
	response_packet->set_status_message("Moved Permanently");
	response_packet->setHeader("Location", location);
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> autoindex_response(std::string &index, std::shared_ptr<ResponsePacket> response_packet)
{
	response_packet->set_status_code(200);
	response_packet->set_status_message("OK");
	response_packet->setHeader("Content-Type", "text/html");
	response_packet->setContent(index);
	return response_packet;
}

std::shared_ptr<ResponsePacket> forbidden()
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(403);
	response_packet->set_status_message("Forbidden");
	response_packet->set_final_response();
	return response_packet;
}

std::shared_ptr<ResponsePacket> created(const std::string &location)
{
	auto response_packet = std::make_unique<ResponsePacket>();
	response_packet->set_status_code(201);
	response_packet->set_status_message("Created");
	response_packet->setHeader("Location", location);
	response_packet->set_final_response();
	return response_packet;
}
