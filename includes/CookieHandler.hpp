#pragma once

#include <map>
#include <string>
#include <unordered_map>

#include "Session.hpp"

class CookieHandler
{
public:
	// singleton
	static CookieHandler &getInstance();
	CookieHandler(const CookieHandler &) = delete;
	CookieHandler &operator=(const CookieHandler &) = delete;

	void addCookie(const std::string &key, const std::string &value);
	std::pair<std::string, std::string> getCookie(const std::string &key); // do i really need the pair instead of just the value?

	std::string createSession();
	bool isValidSession(const std::string &key);
	Session getSession(const std::string &key);

private:
	CookieHandler() = default;
	std::unordered_map<std::string, Session> _sessions;
	std::unordered_map<std::string, std::string> _cookies;
};
