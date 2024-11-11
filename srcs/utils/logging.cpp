#include "logging.hpp"
#include <iomanip>
#include <sstream>

std::string get_timestamp()
{
	// get current times
	auto now = std::chrono::system_clock::now();

	// convert to time_t
	auto now_c = std::chrono::system_clock::to_time_t(now);

	// convert time_t to string
	std::stringstream ss;
	ss << std::put_time(std::localtime(&now_c), "%H:%M:%S");
	return ss.str();
}

void log(const std::string &message, LogLevel level)
{
	std::string level_str;
	if (level < LOG_LEVEL)
		return;
	switch (level)
	{
	case TRACE:
		level_str = "[\033[94mTRACE\033[0m]"; // Light Blue
		break;
	case DEBUG:
		level_str = "[\033[36mDEBUG\033[0m]"; // Cyan
		break;
	case INFO:
		level_str = "[\033[32mINFO\033[0m]"; // Green
		break;
	case WARNING:
		level_str = "[\033[33mWARNING\033[0m]"; // Yellow
		break;
	case ERROR:
		level_str = "[\033[31mERROR\033[0m]"; // Red
		break;
	}
	std::string timestamp = get_timestamp();
	std::cout << "[" << timestamp << "]:" << std::left << std::setw(20) << level_str << message << std::endl;
}
