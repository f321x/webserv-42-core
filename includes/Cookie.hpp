#pragma once

#include <ctime>
#include <string>
#include <functional>

class Cookie
{
public:
	Cookie(const std::string &type);

	time_t getExpires() const;
	std::string getKey() const;
	std::string getType() const;
	std::string getSetCookieHeader() const;

private:
	time_t _expires;
	std::string _key; // do i need this when i store the key in the map?
	std::string _type;
	std::string generateUniqueKey() const;

	const size_t _expirationDuration = 3600; // 1 hour
};