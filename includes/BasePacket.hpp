#pragma once

#include <string>
#include <map>

class BasePacket
{
public:
	BasePacket();
	BasePacket(const BasePacket &other);
	BasePacket &operator=(const BasePacket &other);
	virtual ~BasePacket();

	std::string get_header(const std::string &key) const;
	std::map<std::string, std::string> get_headers();
	std::string get_content() const;

	void set_header(const std::string key, const std::string value);
	void set_content(const std::string content);

protected:
	std::map<std::string, std::string> _headers;
	std::string _content;
	size_t _content_length_header = 0;
};
