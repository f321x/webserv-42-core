#pragma once

#include "BasePacket.hpp"
#include "Utils.hpp"

class RequestPacket : public BasePacket
{
public:
	RequestPacket();
	RequestPacket(const std::string &raw_packet);
	RequestPacket(const RequestPacket &other);
	RequestPacket &operator=(const RequestPacket &other);
	~RequestPacket();

	std::string get_http_version() const;
	std::string get_uri() const;
	Method get_method() const;

	// ToDo: move this to utils
	std::string getPureHostname() const;

	class InvalidPacketException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Invalid packet";
		}
	};

private:
	BasePacket _base_packet;

	std::string _raw_packet;
	Method _method;
	std::string _uri;
	std::string _http_version;

	void parseRawPacket();
};
