#include "Utils.hpp"

std::vector<std::string> split(const std::string &str, const char delim)
{
	std::istringstream			stream(str);
	std::string					token;
	std::vector<std::string>	tokens;

	while (std::getline(stream, token, delim))
	{
		tokens.push_back(token);
	}

	return tokens;
}
