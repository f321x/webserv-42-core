#include "RequestPacket.hpp"
#include "logging.hpp"

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

std::string RequestPacket::parseUri(const std::string &uri)
{
	DEBUG("parseUri: " + uri);
	size_t qPos = uri.find('?');
	if (qPos == std::string::npos)
	{
		_query_string = "";
		return uri;
	}
	DEBUG("? postion: " + std::to_string(qPos));
	_query_string = uri.substr(qPos + 1);
	DEBUG("query string: " + _query_string);
	DEBUG("uri: " + uri.substr(0, qPos));
	return uri.substr(0, qPos);
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

			// _uri = parseUri(tokens[1]);
			std::tie(_uri, _query_tokens) = _parse_request_uri(tokens[1]);
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
		setHeader(trim(key), trim(value));
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
		return;

	// headers can be upper and lowercase
	std::string contentLengthString = getHeader("Content-Length");
	if (contentLengthString == "")
		contentLengthString = getHeader("content-length");
	// removed return because there are also packets without content-length header
	if (contentLengthString != "")
	{
		try
		{
			_content_length_header = std::stoul(contentLengthString);
		}
		catch (const std::exception &e)
		{
			throw InvalidPacketException();
		}
	}

	// // this does not work if there is no content length header
	// if (bodyStart + iOffset + _content_length_header > _raw_packet.length())
	// {
	// 	throw InvalidPacketException();
	// }
	setContent(_raw_packet.substr(bodyStart + iOffset));
}

// parse the uri in a pure uri path and a hashset of query tokens
std::pair<std::string, std::unordered_map<std::string, std::string>> RequestPacket::_parse_request_uri(const std::string &uri)
{
	std::string path = uri;
	std::unordered_map<std::string, std::string> query_tokens;

	size_t question_mark_pos = uri.find('?');
	if (question_mark_pos != std::string::npos)
	{
		path = uri.substr(0, question_mark_pos);
		std::string query = uri.substr(question_mark_pos + 1);
		std::vector<std::string> query_pairs = split(query, '&');
		for (const std::string &query_pair : query_pairs)
		{
			std::vector<std::string> query_token = split(query_pair, '=');
			if (query_token.size() == 2)
			{
				query_tokens[query_token[0]] = query_token[1];
			}
		}
	}

	return std::make_pair(path, query_tokens);
}

int RequestPacket::getContentLengthHeader() const
{
	return _content_length_header;
}

bool RequestPacket::isChunked() const
{
	std::string transferEncoding = getHeader("Transfer-Encoding");
	if (transferEncoding == "")
	{
		transferEncoding = getHeader("transfer-encoding");
	}
	if (transferEncoding == "")
	{
		return false;
	}

	return transferEncoding == "chunked";
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
	_content += new_content;
}