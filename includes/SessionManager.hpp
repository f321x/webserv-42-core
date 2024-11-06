#pragma once

#include <map>
#include <string>
#include <map>

#include "Cookie.hpp"

class SessionManager
{
public:
	// singleton
	static SessionManager &getInstance();
	SessionManager(const SessionManager &) = delete;
	SessionManager &operator=(const SessionManager &) = delete;

	std::string createSession();
	// void delete_cookie(const std::string &key);
	// std::string get_cookie(const std::string &key);

	bool isValidSession(const std::string &key);

private:
	SessionManager() = default;
	std::unordered_map<std::string, Cookie> _cookies;
};