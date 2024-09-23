#include "RequestPacket.hpp"

RequestPacket::RequestPacket()
{
	_raw_packet = "";
	_method = GET;
	_uri = "";
	_http_version = "";
}

RequestPacket::RequestPacket(const std::string &raw_packet)
{
	_raw_packet = raw_packet;
	parseRawPacket();
}

RequestPacket::RequestPacket(const RequestPacket &other) : BasePacket(other)
{
	*this = other;
}

RequestPacket &RequestPacket::operator=(const RequestPacket &other)
{
	_raw_packet = other._raw_packet;
	_method = other._method;
	_uri = other._uri;
	_http_version = other._http_version;
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

void RequestPacket::parseRawPacket()
{
	std::vector<std::string> lines = split(_raw_packet, '\n');

	for (size_t lInd = 0; lInd < lines.size(); lInd++)
	{
		if (lInd == 0)
		{
			// Request line
			std::vector<std::string> tokens = split(lines[lInd], ' ');
			if (tokens.size() != 3)
			{
				throw InvalidPacketException();
			}

			if (tokens[0] == "GET")
			{
				_method = GET;
			}
			else if (tokens[0] == "POST")
			{
				_method = POST;
			}
			else if (tokens[0] == "DELETE")
			{
				_method = DELETE;
			}
			else
			{
				throw UnknownMethodException();
			}

			_uri = tokens[1];
			_http_version = trim(tokens[2]);
			continue;
		}
		else if (trim(lines[lInd]).empty())
		{
			break;
		}

		size_t colonPos = lines[lInd].find(':');
		if (colonPos == std::string::npos)
		{
			continue;
		}

		std::string key = lines[lInd].substr(0, colonPos);
		std::string value = lines[lInd].substr(colonPos + 1);
		if (key.length() == 0 || value.length() == 0)
		{
			continue;
		}
		set_header(trim(key), trim(value));
	}

	std::string contentLengthString = get_header("Content-Length");
	if (contentLengthString == "")
	{
		return;
	}

	// Body
	size_t bodyStart = _raw_packet.find("\n\n");
	unsigned int iOffset = 2;
	if (bodyStart == std::string::npos)
	{
		bodyStart = _raw_packet.find("\r\n\r\n");
		iOffset = 4;
	}
	if (bodyStart == std::string::npos)
	{
		return;
	}

	size_t contentLength;
	try
	{
		contentLength = std::stoul(contentLengthString);
	}
	catch (const std::exception &e)
	{
		throw InvalidPacketException();
	}

	if (bodyStart + iOffset + contentLength > _raw_packet.length())
	{
		throw InvalidPacketException();
	}
	set_content(_raw_packet.substr(bodyStart + iOffset));
}

int RequestPacket::get_content_length_header() const
{
	std::string contentLengthString = get_header("Content-Length");
	if (contentLengthString == "")
	{
		return 0;
	}

	try
	{
		return std::stoi(contentLengthString);
	}
	catch (const std::exception &e)
	{
		return 0;
	}
}

bool RequestPacket::is_chunked() const
{
	std::string transferEncoding = get_header("Transfer-Encoding");
	if (transferEncoding == "")
	{
		return false;
	}

	return transferEncoding == "chunked";
}

size_t RequestPacket::get_content_size() const
{
	return _content.length();
}
