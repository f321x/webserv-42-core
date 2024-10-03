#pragma once

#include "BasePacket.hpp"
#include "Utils.hpp"

class RequestPacket : public BasePacket
{
public:
	RequestPacket();
	RequestPacket(const std::string &raw_packet);
	RequestPacket(const RequestPacket &other);
	RequestPacket &operator=(const RequestPacket &other);
	~RequestPacket();

	std::string get_http_version() const;
	std::string get_uri() const;
	Method get_method() const;
	int get_content_length_header() const;
	bool is_chunked() const;
	size_t get_content_size() const;
	void replace_content(const std::string &new_content);
	void add_to_content(const std::string &new_content);

	class InvalidPacketException : public std::exception
	{
		const char *what() const noexcept override
		{
			return "Invalid packet";
		}
	};
	class UnknownMethodException : public std::exception
	{
		const char *what() const noexcept override
		{
			return "Unknown method";
		}
	};

private:
	std::string _raw_packet;
	Method _method;
	std::string _uri;
	std::string _http_version;
	std::unordered_map<std::string, std::string> _query_tokens;

	void parseRawPacket();
	std::pair<std::string, std::unordered_map<std::string, std::string>> _parse_request_uri(const std::string &uri);
};
