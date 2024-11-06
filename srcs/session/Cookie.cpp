#include "Cookie.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

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

// static std::string formatExpirationTime(time_t expiresAt)
// {
// 	std::tm *gmt = std::gmtime(&expiresAt);
// 	char buffer[30];
// 	std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", gmt);
// 	return std::string(buffer);
// }

std::string Cookie::getSetCookieHeaderValue() const
{
	// Base structure for Set-Cookie header
	std::string header = _type + "=" + _key + "; Max-Age=" + std::to_string(_expirationDuration);
	header += "; Path=/";	// Cookie is valid for the entire site
	header += "; HttpOnly"; // Makes cookie inaccessible to JavaScript (useful for session cookies)
	return header;
}
