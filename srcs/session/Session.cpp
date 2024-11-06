#include "Session.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

std::string Session::generateUniqueKey() const
{
	return std::to_string(std::hash<std::string>{}(std::to_string(std::time(nullptr))));
}

Session::Session(const std::string &type)
{
	_key = generateUniqueKey();
	_type = type;
	this->_expires = std::time(nullptr) + _expirationDuration;
}

time_t Session::getExpires() const
{
	return _expires;
}

std::string Session::getKey() const
{
	return _key;
}

std::string Session::getType() const
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

std::string Session::getSetSessionHeaderValue() const
{
	// Base structure for Set-Session header
	std::string header = _type + "=" + _key + "; Max-Age=" + std::to_string(_expirationDuration);
	header += "; Path=/";	// Session is valid for the entire site
	header += "; HttpOnly"; // Makes Session inaccessible to JavaScript (useful for session Sessions)
	return header;
}
