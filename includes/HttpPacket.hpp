#pragma once

#include <cstring>
#include <map>

enum Method
{
	GET,
	POST,
	DELETE
};

class HttpPacket
{
public:
	HttpPacket();							   // construct empty packet
	HttpPacket(const std::string &raw_packet); // deserialize raw packet
	HttpPacket(const HttpPacket &other);
	HttpPacket &operator=(const HttpPacket &other);
	~HttpPacket();

	std::string serializeResponse();
private:
	std::string	_raw_packet;

	// Request
	Method		_method;
	std::string	_uri;
	std::string	_http_version;

	// Response
	uint		_status_code;
	std::string	_status_message;

	std::map<std::string, std::string>	_request_headers;
	std::map<std::string, std::string>	_response_headers;

	void parseRawPacket();
};
