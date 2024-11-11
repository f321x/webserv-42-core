#include "RequestPacket.hpp"
#include "logging.hpp"

RequestPacket::RequestPacket() : _max_body_size(0)
{
	_buffer = "";
	_parsed_header = false;
	_method = GET;
	_uri = "";
	_http_version = "";
}

RequestPacket::RequestPacket(size_t max_body_size) : _max_body_size(max_body_size)
{
	_buffer = "";
	_parsed_header = false;
	_method = GET;
	_uri = "";
	_http_version = "";
}

RequestPacket::RequestPacket(const RequestPacket &other) : BasePacket(other), _max_body_size(other._max_body_size)
{
	*this = other;
}

RequestPacket &RequestPacket::operator=(const RequestPacket &other)
{
	_buffer = other._buffer;
	_parsed_header = other._parsed_header;
	_method = other._method;
	_uri = other._uri;
	_http_version = other._http_version;
	return *this;
}

RequestPacket::~RequestPacket() = default;

std::string RequestPacket::getHttpVersion() const
{
	return _http_version;
}

std::string RequestPacket::getUri() const
{
	return _uri;
}

Method RequestPacket::getMethod() const
{
	return _method;
}

size_t RequestPacket::getContentLengthHeader() const
{
	return _content_length_header;
}

size_t RequestPacket::getContentSize() const
{
	return _content.length();
}

std::string RequestPacket::getQueryString() const
{
	return _query_string;
}

void RequestPacket::replaceContent(const std::string &new_content)
{
	_content = new_content;
}

void RequestPacket::addToContent(const std::string &new_content)
{
	if (_content.size() + new_content.size() > _max_body_size)
		throw InvalidPacketException();
	_content += new_content;
}

