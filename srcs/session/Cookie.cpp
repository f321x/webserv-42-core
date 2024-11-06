#include "Cookie.hpp"

std::string Cookie::generateUniqueKey() const
{
	return std::to_string(std::hash<std::string>{}(std::to_string(std::time(nullptr))));
}

Cookie::Cookie(const std::string &type)
{
	_key = generateUniqueKey();
	_type = type;
	this->_expires = std::time(nullptr) + _expirationDuration;
}

time_t Cookie::getExpires() const
{
	return _expires;
}

std::string Cookie::getKey() const
{
	return _key;
}

std::string Cookie::getType() const
{
	return _type;
}

std::string Cookie::getSetCookieHeader() const
{
	return "Set-Cookie: " + _key + "=" + _type + "; Expires=" + std::to_string(_expires) + "\r\n";
}
