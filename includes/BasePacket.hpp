#pragma once

#include <string>
#include <map>
#include "Utils.hpp"

class BasePacket
{
public:
	BasePacket();
	BasePacket(const BasePacket &other);
	BasePacket &operator=(const BasePacket &other);
	~BasePacket();

	std::string get_req_header(const std::string &key) const;
	std::map<std::string, std::string> get_req_headers() const;
	std::string get_content() const;

	void set_res_header(const std::string key, const std::string value);
	void set_content(const std::string content);

protected:
	std::map<std::string, std::string> _headers;
	std::string _content;
};
