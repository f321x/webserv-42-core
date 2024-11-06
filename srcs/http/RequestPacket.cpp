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
	// Find the end of headers
	size_t headers_end = _raw_packet.find("\r\n\r\n");
	if (headers_end == std::string::npos)
	{
		throw InvalidPacketException();
	}

	// Extract headers
	std::string headers_part = _raw_packet.substr(0, headers_end);
	std::istringstream headers_stream(headers_part);

	std::string line;
	bool first_line = true;
	while (std::getline(headers_stream, line))
	{
		// Remove '\r' at the end if present
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		if (first_line)
		{
			// Parse the request line
			std::istringstream request_line_stream(line);
			std::string method_str, uri_str, version_str;
			request_line_stream >> method_str >> uri_str >> version_str;

			// Set method
			if (method_str == "GET")
				_method = GET;
			else if (method_str == "POST")
				_method = POST;
			else if (method_str == "DELETE")
				_method = DELETE;
			else
				throw UnknownMethodException();

			// Parse URI
			std::tie(_uri, _query_tokens) = _parse_request_uri(uri_str);
			_http_version = version_str;

			first_line = false;
		}
		else if (line.empty())
		{
			// Should not happen here since we split until headers_end
			break;
		}
		else
		{
			// Parse header
			size_t colon_pos = line.find(':');
			if (colon_pos != std::string::npos)
			{
				std::string key = line.substr(0, colon_pos);
				std::string value = line.substr(colon_pos + 1);
				DEBUG("Header:" + key + "=" + value);
				setHeader(trim(key), trim(value));
			}
		}
	}

	// Body starts after the headers_end marker
	size_t body_start = headers_end + 4; // Skip past "\r\n\r\n"
	if (body_start < _raw_packet.size())
	{
		// Extract the body content
		std::string body = _raw_packet.substr(body_start);
		DEBUG("Body: " + body);
		setContent(body);

		// Parse Content-Length if present
		std::string content_length_str = getHeader("Content-Length");
		if (!content_length_str.empty())
		{
			try
			{
				_content_length_header = std::stoul(content_length_str);
			}
			catch (const std::exception &e)
			{
				throw InvalidPacketException();
			}
		}
	}
	setSessionId("dummy");
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

std::string RequestPacket::getSessionId() const
{
	return _session_id;
}

void RequestPacket::setSessionId(const std::string &session_id)
{
	_session_id = session_id;
}