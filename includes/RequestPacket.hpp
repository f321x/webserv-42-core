#pragma once

#include "BasePacket.hpp"
#include "Utils.hpp"

class RequestPacket : public BasePacket
{
public:
	RequestPacket(size_t max_body_size);
	RequestPacket(const RequestPacket &other);
	RequestPacket &operator=(const RequestPacket &other);
	~RequestPacket();

	std::string getHttpVersion() const;
	std::string getUri() const;
	Method getMethod() const;
	size_t getContentLengthHeader() const;
	bool isChunked() const;
	size_t getContentSize() const;
	std::string getQueryString() const;
	void replaceContent(const std::string &new_content);
	void addToContent(const std::string &new_content);

	bool append(const std::string &data);

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
	RequestPacket();

	std::string _buffer;
	bool _parsed_header;
	Method _method;
	std::string _uri;
	std::string _query_string;
	std::string _http_version;
	const size_t _max_body_size;
	std::string _parseUri(const std::string &uri);
	void _parseContentLength();
	std::unordered_map<std::string, std::string> _query_tokens;
	std::pair<std::string, std::unordered_map<std::string, std::string>> _parseRequestUri(const std::string &uri);

	bool _appendHeader();
	bool _validFirstLine(const std::string &line);
	bool _appendContent();
	bool _appendChunkedData();
};
