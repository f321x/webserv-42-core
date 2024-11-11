#include "ResponsePacket.hpp"
#include "logging.hpp"

ResponsePacket::ResponsePacket()
{
	_status_code = 200;
	_status_message = "OK";
	_final_response = false;
}

ResponsePacket::ResponsePacket(const std::string &cgi_response) : BasePacket(), _status_code(200), _status_message("OK"), _final_response(false)
{
	bool crlf = true;
	// Separate headers from body
	size_t header_end = cgi_response.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		header_end = cgi_response.find("\n\n"); // Try for LF if no CRLF
		crlf = false;
		DEBUG("LF found");
	}

	std::string headers_part;
	std::string body_part;

	if (header_end != std::string::npos)
	{
		headers_part = cgi_response.substr(0, header_end);
		body_part = cgi_response.substr(header_end + (crlf ? 4 : 2)); // Move past \r\n\r\n or \n\n
	}
	else
		throw std::runtime_error("Invalid CGI response: no headers found");

	// Parse headers
	std::istringstream headers_stream(headers_part);
	std::string line;

	while (std::getline(headers_stream, line))
	{
		if (line.empty())
			continue; // Skip empty lines

		if (line.back() == '\r')
			line.pop_back(); // Remove trailing \r
		// Split the line into key and value for headers
		size_t delimiter = line.find(':');
		if (delimiter != std::string::npos)
		{
			std::string key = line.substr(0, delimiter);
			std::string value = line.substr(delimiter + 1);

			// Trim whitespace from the value
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t") + 1);

			// If we find the Status header, extract the status code and message
			if (key == "Status")
			{
				size_t space_pos = value.find(' ');
				if (space_pos != std::string::npos)
				{
					_status_code = std::stoi(value.substr(0, space_pos));
					_status_message = value.substr(space_pos + 1);
				}
				continue;
			}
			// Set the header in the base class
			DEBUG("Setting header: " + key + ":" + value + ":");
			setHeader(key, value);
		}
	}
	if (getHeader("Content-Type").empty())
		throw std::runtime_error("Invalid CGI response: no Content-Type header found");
	DEBUG("trying to get Content-Type: " + getHeader("Content-Type"));
	// Set content
	setContent(body_part);
}

ResponsePacket::ResponsePacket(const ResponsePacket &other) : BasePacket(other)
{
	*this = other;
}

ResponsePacket &ResponsePacket::operator=(const ResponsePacket &other)
{
	_status_code = other._status_code;
	_status_message = other._status_message;
	_final_response = other._final_response;
	return *this;
}

ResponsePacket::~ResponsePacket() = default;

void ResponsePacket::set_status_code(uint status_code)
{
	_status_code = status_code;
}

void ResponsePacket::set_status_message(const std::string &status_message)
{
	_status_message = status_message;
}

void ResponsePacket::set_final_response()
{
	_final_response = true;
}

bool ResponsePacket::is_final_response() const
{
	return _final_response;
}

// bool ResponsePacket::getResponseReady() const
// {
// 	return _response_ready.load();
// }

// void ResponsePacket::setResponseReady(bool flag)
// {
// 	_response_ready = flag;
// }

std::string ResponsePacket::serialize()
{
	setHeader("Content-Length", std::to_string(_content.size()));

	std::string response = "HTTP/1.1 " + std::to_string(_status_code) + " " + _status_message + "\n";

	for (auto &_header : _headers)
	{
		response += _header.first + ": " + _header.second + "\n";
	}
	response += "\n";

	response += _content;

	return response;
}