#include "CookieHandler.hpp"

CookieHandler &CookieHandler::getInstance()
{
	static CookieHandler instance;
	return instance;
}

std::string CookieHandler::createSession()
{
	Cookie new_cookie("session");
	std::string key = new_cookie.getKey();
	_cookies.emplace(new_cookie.getKey(), std::move(new_cookie));
	return key;
}

bool CookieHandler::isValidSession(const std::string &key)
{
	// check if the cookies exists and if expired
	auto it = _cookies.find(key);
	if (it == _cookies.end())
		return false;
	if (it->second.getExpires() < std::time(nullptr))
	{
		_cookies.erase(it);
		return false;
	}
	return true;
}

Cookie CookieHandler::getSessionCookie(const std::string &key)
{
	return _cookies.at(key);
}