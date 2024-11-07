#pragma once

#include <string>
#include <map>
#include <unordered_map>

class BasePacket
{
public:
	BasePacket();
	BasePacket(const BasePacket &other);
	BasePacket &operator=(const BasePacket &other);
	virtual ~BasePacket();

	std::string getHeader(const std::string &key) const;
	std::map<std::string, std::string> getHeaders();
	std::string getContent() const;
	std::string getBoundary() const;

	void setHeader(const std::string& key, const std::string& value);
	void setContent(const std::string& content);

protected:
	std::map<std::string, std::string> _headers;
	std::string _boundary;
	std::string _content;
	size_t _content_length_header = 0;
};
