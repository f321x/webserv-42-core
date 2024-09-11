#include <catch2/catch_test_macros.hpp>
#include <fstream>  // For std::ofstream
#include <cstdio>   // For std::remove to delete the temporary file
#include "WebServerConfig.hpp"   // Include your config classes
#include "ServerConfig.hpp"
#include "RouteConfig.hpp"

// Helper function to create a temporary config file
std::string createTempConfigFile(const std::string& config_data) {
    std::string temp_filename = "temp_config.cfg";  // You can make this more random if needed
    std::ofstream temp_file(temp_filename);
    temp_file << config_data;
    temp_file.close();
    return temp_filename;
}

// Helper function to remove the temporary config file after the test
void removeTempConfigFile(const std::string& filename) {
    std::remove(filename.c_str());
}

TEST_CASE("WebServerConfig Parses Server Configurations Correctly", "[WebServerConfig]") {
    std::string config_data = R"(
        server {
            listen 8080;
            server_name example.com;
            client_max_body_size 1048576;
            location / {
                root /var/www;
                autoindex on;
                methods GET POST;
                default_file index.html;
            }
        }
    )";

    std::string temp_filename = createTempConfigFile(config_data);

    // Pass the filename to the WebServerConfig constructor
    WebServerConfig webConfig(temp_filename);

    // Verify the server configurations were parsed correctly
    const ServerConfig& serverConfig = webConfig.getServerConfig();
    
    REQUIRE(serverConfig.getPort() == 8080);
    REQUIRE(serverConfig.getServerNames() == std::vector<std::string>{"example.com"});
    REQUIRE(serverConfig.getClientMaxBodySize() == 1048576);
    // Verify route configurations
	const auto& routes = serverConfig.getRoutes();
	try {
		const RouteConfig& routeConfig = routes.at("/");
		REQUIRE(routeConfig.getRoot() == "/var/www");
		REQUIRE(routeConfig.isAutoindex() == true);
		REQUIRE(routeConfig.getAcceptedMethods() == std::vector<std::string>{"GET", "POST"});
		REQUIRE(routeConfig.getDefaultFile() == "index.html");
	} catch (const std::out_of_range& e) { FAIL("Route not found"); }

    // Clean up the temporary file
    removeTempConfigFile(temp_filename);
}

TEST_CASE("WebServerConfig Throws Exception on Invalid Configuration", "[WebServerConfig]") {
    std::string invalid_config_data = R"(
        server {
            listen not_a_port;
            server_name example.com;
        }
    )";

    std::string temp_filename = createTempConfigFile(invalid_config_data);

    // Expect the parser to throw an exception due to the invalid port
    REQUIRE_THROWS_AS(WebServerConfig(temp_filename), std::runtime_error);

    // Clean up the temporary file
    removeTempConfigFile(temp_filename);
}

TEST_CASE("WebServerConfig Parses Multiple Routes", "[WebServerConfig]") {
    std::string config_data = R"(
        server {
            listen 8080;
            location /site1 {
                root /var/www/site1;
                autoindex off;
                methods GET;
            }
            location /site2 {
                root /var/www/site2;
                autoindex on;
                methods POST;
            }
        }
    )";

    std::string temp_filename = createTempConfigFile(config_data);

    WebServerConfig webConfig(temp_filename);
    const ServerConfig& serverConfig = webConfig.getServerConfig();

    REQUIRE(serverConfig.getPort() == 8080);

	const auto& routes = serverConfig.getRoutes();
    const RouteConfig& route1 = routes.at("/site1");
    REQUIRE(route1.getRoot() == "/var/www/site1");
    REQUIRE(route1.isAutoindex() == false);
    REQUIRE(route1.getAcceptedMethods() == std::vector<std::string>{"GET"});

    const RouteConfig& route2 = routes.at("/site2");
    REQUIRE(route2.getRoot() == "/var/www/site2");
    REQUIRE(route2.isAutoindex() == true);
    REQUIRE(route2.getAcceptedMethods() == std::vector<std::string>{"POST"});

    // Clean up the temporary file
    removeTempConfigFile(temp_filename);
}

TEST_CASE("WebServerConfig Handles Missing Required Fields", "[WebServerConfig]") {
    std::string incomplete_config_data = R"(
        server {
            location / {
                autoindex on;
            }
        }
    )";

    std::string temp_filename = createTempConfigFile(incomplete_config_data);

    // Expect an exception due to missing 'root' field
    REQUIRE_THROWS_AS(WebServerConfig(temp_filename), std::runtime_error);

    // Clean up the temporary file
    removeTempConfigFile(temp_filename);
}

TEST_CASE("WebServerConfig Handles Redirection Parsing", "[WebServerConfig]") {
    std::string config_data = R"(
        server {
            listen 8080;
            location /redirect {
                redirection http://example.com;
            }
        }
    )";

    std::string temp_filename = createTempConfigFile(config_data);

    WebServerConfig webConfig(temp_filename);
    const ServerConfig& serverConfig = webConfig.getServerConfig();

	const auto& routes = serverConfig.getRoutes();
    const RouteConfig& route = routes.at("/redirect");
    REQUIRE(route.getRedirection() == "http://example.com");

    // Clean up the temporary file
    removeTempConfigFile(temp_filename);
}