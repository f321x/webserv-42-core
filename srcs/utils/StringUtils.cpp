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

void remove_content_from_packet(std::string &packet, const std::string &content)
{
	const size_t content_start = packet.find(content);
	if (content_start == std::string::npos)
		return;
	packet.erase(content_start, content.size());
}

std::string &toLowerCaseInPlace(std::string &str)
{
	const std::locale loc;

	for (auto &elem : str)
		elem = std::tolower(elem);

	return str;
}
