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

	std::string getHttpVersion() const;
	std::string getUri() const;
	Method getMethod() const;
	int getContentLengthHeader() const;
	bool isChunked() const;
	size_t getContentSize() const;
	std::string getQueryString() const;
	void replaceContent(const std::string &new_content);
	void addToContent(const std::string &new_content);

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
	std::string _boundary;
	void parseRawPacket();
};
