#include <catch2/catch_test_macros.hpp>
#include "RouteConfig.hpp"  // Include the header file for your RouteConfig class



TEST_CASE("RouteConfig Default Values", "[RouteConfig]") {
    RouteConfig routeConfig;

    // Test default values
    REQUIRE(routeConfig.getRoot() == "");
    REQUIRE(routeConfig.isAutoindex() == false);
    REQUIRE(routeConfig.isDirectoryListing() == false);
    REQUIRE(routeConfig.getDefaultFile() == "");
}

TEST_CASE("RouteConfig Setting Values", "[RouteConfig]") {
    RouteConfig routeConfig;

    // Set and test root
    routeConfig.setRoot("/var/www");
    REQUIRE(routeConfig.getRoot() == "/var/www");

    // Set and test autoindex
    routeConfig.setAutoindex(true);
    REQUIRE(routeConfig.isAutoindex() == true);

    // Set and test directory listing
    routeConfig.setDirectoryListing(true);
    REQUIRE(routeConfig.isDirectoryListing() == true);

    // Set and test default file
    routeConfig.setDefaultFile("home.html");
    REQUIRE(routeConfig.getDefaultFile() == "home.html");

    // Set and test accepted methods
	std::vector<std::string> methods = {"GET", "POST"};
    routeConfig.setAcceptedMethods(methods);
    REQUIRE(routeConfig.getAcceptedMethods() == methods);
}
