#pragma once

#include <ctime>
#include <string>
#include <functional>

class Session
{
public:
	Session(const std::string &type);

	time_t getExpires() const;
	std::string getKey() const;
	std::string getType() const;
	std::string getSetSessionHeaderValue() const;

private:
	time_t _expires;
	std::string _key; // do i need this when i store the key in the map?
	std::string _type;
	std::string generateUniqueKey() const;

	const time_t _expirationDuration = 3600; // 1 hour
};