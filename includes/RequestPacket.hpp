#pragma once

#include "BasePacket.hpp"
#include "Utils.hpp"

class RequestPacket : public BasePacket
{
public:
	RequestPacket();
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
	std::string _buffer;
	bool _parsed_header;
	Method _method;
	std::string _uri;
	std::string _query_string;
	std::string _http_version;
	std::string parseUri(const std::string &uri);
	void parseContentLenght();
	std::unordered_map<std::string, std::string> _query_tokens;
	std::pair<std::string, std::unordered_map<std::string, std::string>> _parse_request_uri(const std::string &uri);

	bool appendHeader();
	bool appendContent();
	bool appendChunkedData();
};
