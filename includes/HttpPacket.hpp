#pragma once

#include <string>
#include <map>

enum Method
{
	GET,
	POST,
	DELETE
};

class HttpPacket
{
public:
	HttpPacket();							   // construct empty packet
	HttpPacket(const std::string &raw_packet); // deserialize raw packet
	HttpPacket(const HttpPacket &other);
	HttpPacket &operator=(const HttpPacket &other);
	~HttpPacket();

	std::string	get_http_version() const;
	std::string	get_uri() const;
	Method		get_method() const;
	std::string	get_req_header(const std::string &key) const;
	std::map<std::string, std::string> get_req_headers() const;

	void		set_status_code(uint status_code);
	void		set_status_message(const std::string status_message);
	void		set_res_header(const std::string key, const std::string value);
	void		set_content(const std::string content);

	std::string serializeResponse();

	class InvalidPacketException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Invalid packet";
		}
	};
	class UnknownMethodException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Unknown method";
		}
	};
private:
	std::string	_raw_packet;

	// Request
	Method		_method;
	std::string	_uri;
	std::string	_http_version;

	// Response
	uint		_status_code;
	std::string	_status_message;

	std::map<std::string, std::string>	_request_headers;
	std::map<std::string, std::string>	_response_headers;

	std::string	_content;

	void parseRawPacket();
};
