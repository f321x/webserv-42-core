#include <catch2/catch_test_macros.hpp>

#include "HttpPacket.hpp"

TEST_CASE("HttpPacket parsing") {
	SECTION("Request parsing") {
		// Test parsing a simple GET request
		std::string request = "GET /index.html HTTP/1.1\nHost: localhost";
		HttpPacket packet(request);

		REQUIRE(packet.get_method() == GET);
		REQUIRE(packet.get_uri() == "/index.html");
		REQUIRE(packet.get_http_version() == "HTTP/1.1");
		REQUIRE(packet.get_req_header("Host") == "localhost");
	}
}
