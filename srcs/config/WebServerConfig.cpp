#include "WebServerConfig.hpp"

WebServerConfig::WebServerConfig(const std::string &config_file_path)
{
	std::map<std::string, std::function<void(ServerConfig&, std::istringstream&)>> server_setters = {
		{"listen", parseListen},
		{"server_name", parseServerName},
		{"error_page", parseErrorPage},
		{"client_max_body_size", parseClientMaxBodySize}
	};
	std::map<std::string, std::function<void(RouteConfig&, std::istringstream&)>> route_setters = {
		{"root", parseRoot},
		{"methods", parseAcceptedMethods},
		{"redirection", parseRedirection},
		{"autoindex", parseAutoindex},
		{"default_file", parseDefaultFile},
		{"directory_listing", parseDirectoryListing},
		{"upload_directory", parseUploadDirectory}
	};
	std::string line;
	std::ifstream config_file(config_file_path);
	std::stack<std::string> context_stack;
	RouteConfig current_route;
	ServerConfig current_server;

	if (!config_file.is_open())
		throw std::runtime_error("Failed to open config file");
	while (std::getline(config_file, line))
	{
		std::istringstream stream(line);
		std::string token;

		while (stream >> token)
		{
			if (token == "server")
				context_stack.push("server");
			else if (token == "location") {
				std::string root;
				stream >> root;
				current_route.setRoot(root);
				context_stack.push("location");
			} else if (token == "{") {
				// do nothing
			} else if (token == "}") {
				if (context_stack.empty())
					throw std::runtime_error("Unexpected '}'");
				if (context_stack.top() == "location") {
					current_server.addRoute(current_route.getRoot(), current_route);
					current_route = RouteConfig();
				}
				context_stack.pop();
			} else {
				if (context_stack.top() == "server") {
					auto it = server_setters.find(token);
					if (it != server_setters.end())
						it->second(current_server, stream);
					else
						throw std::runtime_error("Unknown server directive: " + token);
				} else if (context_stack.top() == "location") {
					auto it = route_setters.find(token);
					if (it != route_setters.end())
						it->second(current_route, stream);
					else
						throw std::runtime_error("Unknown route directive: " + token);
				}
			}
		}
	}
	if (!context_stack.empty())
		throw std::runtime_error("Unexpected EOF");
	_server_config = current_server;
}

WebServerConfig::~WebServerConfig()
{
	TRACE("WebServerConfig destructor");
}

WebServerConfig::WebServerConfig()
{
	WARN("Using dummy WebServerConfig constructor");
	_bind_ip = "0.0.0.0";
	_bind_port = 8080;
}

WebServerConfig::WebServerConfig(const WebServerConfig &other)
{
	if (this != &other)
	{
		_bind_ip = other._bind_ip;
		_bind_port = other._bind_port;
	}
}

WebServerConfig &WebServerConfig::operator=(const WebServerConfig &other)
{
	(void)other;
	return *this;
}

SocketAddress WebServerConfig::get_bind_address() const
{
	// can throw on invalid args
	SocketAddress address = SocketAddress(_bind_ip, _bind_port);
	return address;
}

ServerConfig WebServerConfig::getServerConfig() const
{
	return _server_config;
}