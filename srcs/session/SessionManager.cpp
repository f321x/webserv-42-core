#include "SessionManager.hpp"

SessionManager &SessionManager::getInstance()
{
	static SessionManager instance;
	return instance;
}

std::string SessionManager::createSession()
{
	Cookie new_cookie("session");
	std::string key = new_cookie.getKey();
	_cookies.emplace(new_cookie.getKey(), std::move(new_cookie));
	return key;
}

bool SessionManager::isValidSession(const std::string &key)
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