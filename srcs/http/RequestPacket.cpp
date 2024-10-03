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

			auto [_uri, _query_tokens] = _parse_request_uri(tokens[1]);
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
	std::string contentLengthString = get_header("Content-Length");
	if (contentLengthString == "")
		contentLengthString = get_header("content-length");
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
	set_content(_raw_packet.substr(bodyStart + iOffset));
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

int RequestPacket::get_content_length_header() const
{
	return _content_length_header;
}

bool RequestPacket::is_chunked() const
{
	std::string transferEncoding = get_header("Transfer-Encoding");
	if (transferEncoding == "")
	{
		transferEncoding = get_header("transfer-encoding");
	}
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

void RequestPacket::replace_content(const std::string &new_content)
{
	_content = new_content;
}

void RequestPacket::add_to_content(const std::string &new_content)
{
	_content += new_content;
}