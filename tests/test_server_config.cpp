#include <catch2/catch_test_macros.hpp>
#include "ServerConfig.hpp" // Include the ServerConfig class

TEST_CASE("ServerConfig Default Values", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Test default port
	REQUIRE(serverConfig.getPort() == -1); // Default is -1 (invalid port)

	// Test default host
	REQUIRE(serverConfig.getHost() == ""); // Assuming default host is an empty string

	// Test default client body limit (assuming 1MB default)
	REQUIRE(serverConfig.getClientMaxBodySize() == 1024 * 1024);

	// Test default server names
	REQUIRE(serverConfig.getServerNames().empty()); // No server names by default

	// Test that no routes are defined by default
	REQUIRE(serverConfig.getRoutes().empty());
}

TEST_CASE("ServerConfig Set and Get Values", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Set and test port
	serverConfig.setPort(8080);
	REQUIRE(serverConfig.getPort() == 8080);

	// Set and test host
	serverConfig.setHost("127.0.0.1");
	REQUIRE(serverConfig.getHost() == "127.0.0.1");

	// Set and test client body limit
	serverConfig.setClientMaxBodySize(2048);
	REQUIRE(serverConfig.getClientMaxBodySize() == 2048);

	// Set and test server names
	std::vector<std::string> serverNames = {"example.com", "www.example.com"};
	for (const auto &serverName : serverNames)
		serverConfig.addServerName(serverName);
	REQUIRE(serverConfig.getServerNames() == serverNames);
}

TEST_CASE("ServerConfig Route Configuration", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Create a route configuration
	RouteConfig routeConfig;
	routeConfig.setRoot("/var/www");
	routeConfig.setAutoindex(true);
	routeConfig.setAcceptedMethods({"GET", "POST"});

	// Add the route and test
	serverConfig.addRoute("/example", routeConfig);
	RouteConfig retrievedRoute = serverConfig.getRoutes().at("/example");

	REQUIRE(retrievedRoute.getRoot() == "/var/www");
	REQUIRE(retrievedRoute.isAutoindex() == true);
	REQUIRE(retrievedRoute.getAcceptedMethods() == std::vector<Method>{GET, POST});
}

TEST_CASE("ServerConfig Client Body Limit Edge Cases", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Test setting a very large body limit
	serverConfig.setClientMaxBodySize(1024 * 1024 * 100); // 100MB
	REQUIRE(serverConfig.getClientMaxBodySize() == 1024 * 1024 * 100);

	// Test setting a small body limit
	serverConfig.setClientMaxBodySize(1); // 1 byte
	REQUIRE(serverConfig.getClientMaxBodySize() == 1);

	// Test setting an invalid body limit (e.g., negative)
	// Assuming your ServerConfig class should throw an exception or have error handling here
	REQUIRE_THROWS_AS(serverConfig.setClientMaxBodySize(-1), std::invalid_argument);
}

TEST_CASE("ServerConfig Handling Multiple Routes", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Create two route configurations
	RouteConfig route1;
	route1.setRoot("/var/www/site1");
	route1.setAutoindex(true);

	RouteConfig route2;
	route2.setRoot("/var/www/site2");
	route2.setAutoindex(false);

	// Add routes and test
	serverConfig.addRoute("/site1", route1);
	serverConfig.addRoute("/site2", route2);

	REQUIRE(serverConfig.getRoutes().at("/site1").getRoot() == "/var/www/site1");
	REQUIRE(serverConfig.getRoutes().at("/site1").isAutoindex() == true);

	REQUIRE(serverConfig.getRoutes().at("/site2").getRoot() == "/var/www/site2");
	REQUIRE(serverConfig.getRoutes().at("/site2").isAutoindex() == false);

	// Check that only two routes exist
	REQUIRE(serverConfig.getRoutes().size() == 2);
}

TEST_CASE("ServerConfig Error Handling", "[ServerConfig]")
{
	ServerConfig serverConfig;

	// Attempt to access a route that doesn't exist
	REQUIRE_THROWS_AS(serverConfig.getRoutes().at("/nonexistent"), std::out_of_range);

	// Setting an invalid port (negative value)
	REQUIRE_THROWS_AS(serverConfig.setPort(-1), std::invalid_argument);

	// Setting an invalid port (port above 65535)
	REQUIRE_THROWS_AS(serverConfig.setPort(70000), std::invalid_argument);

	// Adding a route with an invalid path should throw an exception
	RouteConfig invalidRoute;
	invalidRoute.setRoot("/var/www");
	REQUIRE_THROWS_AS(serverConfig.addRoute("", invalidRoute), std::invalid_argument);
}
