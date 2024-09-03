#pragma once

#include <string>
#include <stdexcept>

class WebServerConfig
{
public:
	WebServerConfig(const std::string &config_file_path);
	~WebServerConfig();
	WebServerConfig(const WebServerConfig &other);
	WebServerConfig &operator=(const WebServerConfig &other);

private:
	WebServerConfig();
};
