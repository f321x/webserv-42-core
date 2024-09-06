#include <catch2/catch_test_macros.hpp>

#include "HttpPacket.hpp"

TEST_CASE("HttpPacket parsing") {
	SECTION("Request parsing") {
		// Test parsing a simple GET request
		std::string request = "GET /index.html HTTP/1.1\r\nHost: localhost:4242\r\n\r\n";
		HttpPacket packet(request);

		REQUIRE(packet.get_method() == GET);
		REQUIRE(packet.get_uri() == "/index.html");
		REQUIRE(packet.get_http_version() == "HTTP/1.1");
		REQUIRE(packet.get_req_header("Host") == "localhost:4242");

		// Test parsing no headers
		request = "GET / HTTP/1.1\r\n\r\n";
		packet = HttpPacket(request);
		REQUIRE(packet.get_method() == GET);
		REQUIRE(packet.get_uri() == "/");
		REQUIRE(packet.get_http_version() == "HTTP/1.1");
		REQUIRE(packet.get_req_headers().size() == 0);
	}
}
