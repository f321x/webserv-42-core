#include "RequestPacket.hpp"

RequestPacket::RequestPacket()
{
	_raw_packet = "";
	_method = GET;
	_uri = "";
	_http_version = "";
	_base_packet = BasePacket();
}

RequestPacket::RequestPacket(const std::string &raw_packet)
{
	_raw_packet = raw_packet;
	_base_packet = BasePacket();
	parseRawPacket();
}

RequestPacket::RequestPacket(const RequestPacket &other)
{
	*this = other;
}

RequestPacket &RequestPacket::operator=(const RequestPacket &other)
{
	_raw_packet = other._raw_packet;
	_method = other._method;
	_uri = other._uri;
	_http_version = other._http_version;
	_base_packet = other._base_packet;
	return *this;
}

RequestPacket::~RequestPacket() {}

std::string RequestPacket::get_http_version() const
{
	return _http_version;
}

std::string RequestPacket::get_uri() const
{
	return _uri;
}

Method RequestPacket::get_method() const
{
	return _method;
}

std::string RequestPacket::getPureHostname() const
{
	std::string host = get_req_header("Host");
	size_t colonPos = host.find(':');
	if (colonPos != std::string::npos)
	{
		host = host.substr(0, colonPos);
	}
	return host;
}
