#include "CookieHandler.hpp"
#include "logging.hpp"

CookieHandler &CookieHandler::getInstance()
{
	static CookieHandler instance;
	return instance;
}

std::string CookieHandler::createSession()
{
	Session session("SessionID");
	std::string key = session.getKey();
	_sessions.emplace(session.getKey(), std::move(session));
	return key;
}

bool CookieHandler::isValidSession(const std::string &key)
{
	// check if the cookies exists and if expired
	auto it = _sessions.find(key);
	if (it == _sessions.end())
		return false;
	if (it->second.getExpires() < std::time(nullptr))
	{
		DEBUG("Session expired");
		_sessions.erase(it);
		return false;
	}
	return true;
}

Session CookieHandler::getSession(const std::string &key)
{
	return _sessions.at(key);
}

void CookieHandler::addCookie(const std::string &key, const std::string &value)
{
	_cookies.emplace(key, value);
}

std::pair<std::string, std::string> CookieHandler::getCookie(const std::string &key)
{
	auto it = _cookies.find(key);
	if (it == _cookies.end())
		return std::make_pair("", "");
	return std::make_pair(it->first, it->second);
}