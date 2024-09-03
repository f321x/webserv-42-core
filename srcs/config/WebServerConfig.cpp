#include "WebServerConfig.hpp"

WebServerConfig::WebServerConfig(const std::string &config_file_path)
{
	(void)config_file_path;
}

WebServerConfig::~WebServerConfig()
{
}

WebServerConfig::WebServerConfig()
{
	throw std::runtime_error("WebServerConfig: default constructor is not allowed");
}

WebServerConfig::WebServerConfig(const WebServerConfig &other)
{
	(void)other;
}

WebServerConfig &WebServerConfig::operator=(const WebServerConfig &other)
{
	(void)other;
}
