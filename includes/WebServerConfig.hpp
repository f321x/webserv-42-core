#pragma once

#include <string>

class WebServerConfig
{
public:
	WebServerConfig(const std::string &config_file_path);
	~WebServerConfig();

private:
	WebServerConfig();
	WebServerConfig(const WebServerConfig &other);
	WebServerConfig &operator=(const WebServerConfig &other);
};
