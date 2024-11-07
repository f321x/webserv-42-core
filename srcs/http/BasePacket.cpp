#include "BasePacket.hpp"
#include "logging.hpp"

BasePacket::BasePacket()
{
	_headers = std::map<std::string, std::string>();
	_content = "";
}

BasePacket::BasePacket(const BasePacket &other)
{
	*this = other;
}

BasePacket &BasePacket::operator=(const BasePacket &other)
{
	_headers = other._headers;
	_content = other._content;
	return *this;
}

BasePacket::~BasePacket() = default;

std::string BasePacket::getHeader(const std::string &key) const
{
	const auto it = _headers.find(key);
	if (it == _headers.end())
	{
		return "";
	}
	return it->second;
}

std::map<std::string, std::string> BasePacket::getHeaders()
{
	return _headers;
}

std::string BasePacket::getContent() const
{
	return _content;
}

void BasePacket::setHeader(const std::string& key, const std::string& value)
{
	std::string new_value = value;
	if (key == "Content-Type")
	{
		const size_t boundary_pos = value.find("; boundary=");
		if (boundary_pos != std::string::npos)
		{
			_boundary = value.substr(boundary_pos + 11);
			new_value = value.substr(0, boundary_pos);
		}
	}

	_headers.insert(std::make_pair(key, new_value));
}

void BasePacket::setContent(const std::string& content)
{
	_content = content;
}

std::string BasePacket::getBoundary() const
{
	return _boundary;
}