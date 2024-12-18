#include "WebServerConfig.hpp"

WebServerConfig::WebServerConfig(const std::string &config_file_path)
{
	std::map<std::string, std::function<void(ServerConfig &, std::istringstream &)>> server_setters = {
		{"listen", parseListen},
		{"server_name", parseServerNames},
		{"error_page", parseErrorPage},
		{"client_max_body_size", parseClientMaxBodySize}};
	std::map<std::string, std::function<void(RouteConfig &, std::istringstream &)>> route_setters = {
		{"root", parseRoot},
		{"methods", parseAcceptedMethods},
		{"redirection", parseRedirection},
		{"autoindex", parseAutoindex},
		{"default_file", parseDefaultFile},
		{"upload_directory", parseUploadDirectory},
		{"cgi", parseCgi}};
	std::string line;
	std::ifstream config_file(config_file_path);
	std::stack<std::string> context_stack;
	// RouteConfig current_route;
	std::pair<std::string, RouteConfig> current_route = std::make_pair(std::string(), RouteConfig());
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
			else if (token == "location")
			{
				std::string root;
				stream >> root;
				current_route.first = root;
				context_stack.push("location");
			}
			else if (token == "{")
			{
				// do nothing
			}
			else if (token == "}")
			{
				if (context_stack.empty())
					throw std::runtime_error("Unexpected '}'");
				else if (context_stack.top() == "location")
				{
					INFO("Adding route: " + current_route.first);
					current_route.second.setRoute(current_route.first);
					current_server.addRoute(current_route.first, current_route.second);
					current_route.first.clear();
					current_route.second = RouteConfig();
				}
				else if (context_stack.top() == "server")
				{
					try
					{
						current_server.checkServerConfig();
					}
					catch (const std::exception &e)
					{
						throw std::runtime_error("Invalid server config: " + std::string(e.what()));
					}
					_server_configs.push_back(current_server);
					current_server = ServerConfig();
				}
				context_stack.pop();
			}
			else
			{
				if (context_stack.top() == "server")
				{
					auto it = server_setters.find(token);
					if (it != server_setters.end())
						it->second(current_server, stream);
					else
						throw std::runtime_error("Unknown server directive: " + token);
				}
				else if (context_stack.top() == "location")
				{
					auto it = route_setters.find(token);
					if (it != route_setters.end())
						it->second(current_route.second, stream);
					else
						throw std::runtime_error("Unknown route directive: " + token);
				}
			}
		}
	}
	if (!context_stack.empty())
		throw std::runtime_error("Unexpected EOF");
}

void WebServerConfig::printConfig() const
{
	for (const auto &server : _server_configs)
	{
		std::cout << "Server config:" << std::endl;
		std::cout << "Listen: " << server.getPort() << std::endl;
		std::cout << "Server names: ";
		for (const auto &name : server.getServerNames())
			std::cout << name << " ";
		std::cout << std::endl;
		std::cout << "Error pages: ";
		for (const auto &[code, page] : server.getErrorPages())
			std::cout << code << " -> " << page << " ";
		std::cout << std::endl;
		std::cout << "Client max body size: " << server.getClientMaxBodySize() << std::endl;
		std::cout << "Routes: ";
		for (const auto &[route, config] : server.getRoutes())
		{
			std::cout << route << " -> ";
			std::cout << "Root: " << config.getRoot() << ", ";
			std::cout << "Methods: ";
			for (const auto &method : config.getAcceptedMethods())
				std::cout << method << ":";
			std::cout << ", ";
			std::cout << "Redirection: " << config.getRedirection() << ", ";
			std::cout << "Autoindex: " << config.isAutoindex() << ", ";
			std::cout << "Default file: " << config.getDefaultFile() << ", ";
			std::cout << "CGI: ";
			for (const auto &[extension, path] : config.getCgi())
				std::cout << extension << " -> " << path << " ";
			std::cout << std::endl;
			std::cout << "Upload directory: " << config.getUploadDirectory() << std::endl;
		}
	}
}

WebServerConfig::~WebServerConfig()
{
	TRACE("WebServerConfig destructor");
}

WebServerConfig::WebServerConfig()
{
	WARN("Using dummy WebServerConfig constructor");
}

WebServerConfig::WebServerConfig(const WebServerConfig &other)
{
	if (this != &other)
		_server_configs = other._server_configs;
}

WebServerConfig &WebServerConfig::operator=(const WebServerConfig &other)
{
	if (this != &other)
		_server_configs = other._server_configs;
	return *this;
}

// SocketAddress WebServerConfig::get_bind_address() const
// {
// 	// can throw on invalid args
// 	SocketAddress address = SocketAddress(_bind_ip, _bind_port);
// 	return address;
// }

std::vector<ServerConfig> WebServerConfig::getServerConfigs() const
{
	return _server_configs;
}
