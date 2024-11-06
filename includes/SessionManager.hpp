#pragma once

#include <map>
#include <string>
#include <map>

#include "Cookie.hpp"

class CookieHandler
{
public:
	// singleton
	static CookieHandler &getInstance();
	CookieHandler(const CookieHandler &) = delete;
	CookieHandler &operator=(const CookieHandler &) = delete;

	std::string createSession();
	// void delete_cookie(const std::string &key);
	// std::string get_cookie(const std::string &key);

	bool isValidSession(const std::string &key);
	Cookie getSessionCookie(const std::string &key);

private:
	CookieHandler() = default;
	std::unordered_map<std::string, Cookie> _cookies;
};