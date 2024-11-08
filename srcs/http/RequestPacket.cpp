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

std::string RequestPacket::_parseUri(const std::string &uri)
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

bool RequestPacket::append(const std::string &data)
{
	if (data.empty())
		throw InvalidPacketException();

	_buffer += data;
	if (!_parsed_header)
	{
		if (!_appendHeader())
			return false;
		_parsed_header = true;
	}

	if (isChunked())
		return _appendChunkedData();
	else
		return _appendContent();
}

bool RequestPacket::_appendHeader()
{
	if (_buffer.size() >= 10)
	{
		if (!_validFirstLine(_buffer))
			throw InvalidPacketException();
	}

	// Find the end of headers
	size_t headers_end = _buffer.find("\r\n\r\n");
	if (headers_end == std::string::npos)
		return false;

	// Extract headers
	std::string headers_part = _buffer.substr(0, headers_end);
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
			std::tie(_uri, _query_tokens) = _parseRequestUri(uri_str);
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

	_parseContentLenght();

	_buffer.erase(0, headers_end + 4); // Skip past "\r\n\r\n"
	return true;
}

bool RequestPacket::_validFirstLine(const std::string &line)
{
	std::istringstream request_line_stream(line);
	std::string method_str, uri_str, version_str;
	request_line_stream >> method_str >> uri_str >> version_str;

	if (method_str.empty() || uri_str.empty() || version_str.empty())
		return false;

	if (method_str != "GET" && method_str != "POST" && method_str != "DELETE")
		return false;

	return true;
}

bool RequestPacket::_appendContent()
{
	if (!getHeader("Content-Length").empty() && getContentLengthHeader() != _buffer.size())
		return false;

	DEBUG("Body: " + _buffer);
	setContent(_buffer);
	_buffer.clear();
	return true;
}

bool RequestPacket::_appendChunkedData()
{
	// find the chunk size
	const size_t indChunkSize = _buffer.find("\r\n");
	if (indChunkSize == std::string::npos)
		return false;

	// parse the chunk size
	const std::string chunkSizeStr = _buffer.substr(0, indChunkSize);
	const size_t chunkSize = std::stoul(chunkSizeStr, nullptr, 16);

	if (chunkSize == std::string::npos)
		throw InvalidPacketException();
	if (chunkSize == 0)
		return true;

	// find the end of the chunk data
	const size_t indChunkEnd = _buffer.find("\r\n", indChunkSize + 2);

	if (indChunkEnd == std::string::npos)
		return false;
	if (indChunkEnd + 2 + chunkSize > _buffer.length())
		throw InvalidPacketException();

	// append the chunk data to the result
	this->addToContent(_buffer.substr(indChunkSize + 2, chunkSize));
	_buffer.erase(0, indChunkEnd + 2);

	return _buffer.empty() ? true : _appendChunkedData();
}

void RequestPacket::_parseContentLenght()
{
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

		if (_content_length_header > _max_body_size)
			throw InvalidPacketException();
	}
}

// parse the uri in a pure uri path and a hashset of query tokens
std::pair<std::string, std::unordered_map<std::string, std::string>> RequestPacket::_parseRequestUri(const std::string &uri)
{
	std::string path = uri;
	std::unordered_map<std::string, std::string> query_tokens;

	size_t question_mark_pos = uri.find('?');
	if (question_mark_pos != std::string::npos)
	{
		path = uri.substr(0, question_mark_pos);
		const std::string query = uri.substr(question_mark_pos + 1);
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

size_t RequestPacket::getContentLengthHeader() const
{
	return _content_length_header;
}

bool RequestPacket::isChunked() const
{
	std::string transfer_encoding = getHeader("Transfer-Encoding");

	return toLowerCaseInPlace(transfer_encoding) == "chunked";
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
