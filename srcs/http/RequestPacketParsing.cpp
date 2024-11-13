#include "RequestPacket.hpp"
#include "logging.hpp"

bool RequestPacket::isChunked() const
{
	std::string transfer_encoding = getHeader("Transfer-Encoding");

	return toLowerCaseInPlace(transfer_encoding) == "chunked";
}

bool RequestPacket::append(const std::string &data)
{
	TRACE("Appending data to request");
	if (data.empty())
	{
		TRACE("Empty data received");
		throw InvalidPacketException();
	}

	_buffer += data;
	if (!_parsed_header)
	{
		TRACE("Appending header");
		if (!_appendHeader())
			return false;
		_parsed_header = true;
	}

	if (isChunked())
	{
		TRACE("Appending to chunked request");
		return _appendChunkedData();
	}

	TRACE("Appending content");
	return _appendContent();
}

bool RequestPacket::_appendHeader()
{
	if (_buffer.size() >= 10)
	{
		if (!_validFirstLine(_buffer))
		{
			TRACE("Invalid first line");
			throw InvalidPacketException();
		}
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

	_parseContentLength();

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
		throw UnknownMethodException();

	return true;
}

bool RequestPacket::_appendContent()
{
	if (!getHeader("Content-Length").empty() && getContentLengthHeader() != _buffer.size())
		return false;

	setContent(_buffer);
	_buffer.clear();
	return true;
}

bool RequestPacket::_appendChunkedData()
{
	try
	{
		// find the chunk size
		const size_t indChunkSize = _buffer.find("\r\n");
		if (indChunkSize == std::string::npos)
			return false;

		// parse the chunk size
		const std::string chunkSizeStr = _buffer.substr(0, indChunkSize);
		const size_t chunkSize = std::stoul(chunkSizeStr, nullptr, 16);

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
	catch (const std::exception &e)
	{
		throw InvalidPacketException();
	}
}

void RequestPacket::_parseContentLength()
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
			throw PayloadTooLargeException();
	}
}

std::string &RequestPacket::_decodeUrl(std::string &url)
{
	size_t pos = 0;
	while ((pos = url.find('%', pos)) != std::string::npos)
	{
		if (pos + 2 >= url.size())
			throw InvalidPacketException();

		try
		{
			const char c = static_cast<char>(std::stoi(url.substr(pos + 1, 2), nullptr, 16));
			url.replace(pos, 3, 1, c);
		}
		catch (const std::exception &e)
		{
			throw InvalidPacketException();
		}

		pos += 1;
	}
	return url;
}

// parse the uri in a pure uri path and a hashset of query tokens
std::pair<std::string, std::unordered_map<std::string, std::string>>
RequestPacket::_parseRequestUri(const std::string &uri)
{
	std::string path = uri;
	_decodeUrl(path);
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