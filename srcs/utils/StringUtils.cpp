#include "Utils.hpp"

std::string trim(const std::string &str)
{
	const char *whitespace = " \t\n\r\f\v";

	size_t first = str.find_first_not_of(whitespace);
	if (std::string::npos == first)
		return str;
	size_t last = str.find_last_not_of(whitespace);
	return str.substr(first, (last - first + 1));
}
