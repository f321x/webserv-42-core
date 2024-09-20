#include "ResponsePacket.hpp"

ResponsePacket::ResponsePacket()
{
	_status_code = 200;
	_status_message = "OK";
	_final_response = false;
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

ResponsePacket::~ResponsePacket() {}

void ResponsePacket::set_status_code(uint status_code)
{
	_status_code = status_code;
}

void ResponsePacket::set_status_message(const std::string status_message)
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

std::string ResponsePacket::serialize()
{
	set_res_header("Content-Length", std::to_string(_content.size()));

	std::string response = "HTTP/1.1 " + std::to_string(_status_code) + " " + _status_message + "\n";

	for (std::map<std::string,
				  std::string>::iterator it = _headers.begin();
		 it != _headers.end();
		 it++)
	{
		response += it->first + ": " + it->second + "\n";
	}
	response += "\n";

	response += _content;

	return response;
}
