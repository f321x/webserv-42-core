#include "BasePacket.hpp"
#include "Utils.hpp"
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

BasePacket::~BasePacket() {}

std::string BasePacket::get_header(const std::string &key) const
{
  std::map<std::string, std::string>::const_iterator it =
      _headers.find(key);
  if (it == _headers.end())
  {
    return "";
  }
  return it->second;
}

std::map<std::string, std::string> BasePacket::get_headers()
{
  return _headers;
}

std::string BasePacket::get_content() const
{
  return _content;
}

void BasePacket::set_header(const std::string key, const std::string value)
{
  _headers.insert(std::make_pair(key, value));
}

void BasePacket::set_content(const std::string content)
{
  _content = content;
}
