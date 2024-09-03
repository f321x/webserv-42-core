#pragma once

#include <string>

class HttpPacket
{
public:
	HttpPacket(const std::string &raw_packet); // deserialize raw packet
	HttpPacket();							   // construct empty packet
	~HttpPacket();
	HttpPacket(const HttpPacket &other);
	HttpPacket &operator=(const HttpPacket &other);

	std::string serialize();
private:
	// packet header parts
	// packet body parts
	// request type
	// etc...
};
