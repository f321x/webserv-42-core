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
	throw std::runtime_error("WebServerConfig: copy constructor is not allowed");
}

WebServerConfig &WebServerConfig::operator=(const WebServerConfig &other)
{
	(void)other;
	throw std::runtime_error("WebServerConfig: assignment operator is not allowed");
	return *this;
}
