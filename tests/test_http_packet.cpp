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

		// Test parsing multiple headers
		request = "POST / HTTP/1.1\r\nHost: localhost:4242\r\nUser-Agent: Mozilla/5.0\r\n\r\n";
		packet = HttpPacket(request);
		REQUIRE(packet.get_method() == POST);
		REQUIRE(packet.get_uri() == "/");
		REQUIRE(packet.get_http_version() == "HTTP/1.1");
		REQUIRE(packet.get_req_header("Host") == "localhost:4242");
		REQUIRE(packet.get_req_header("User-Agent") == "Mozilla/5.0");
	}

	SECTION("Response serialization") {
		// Test serializing a simple response
		HttpPacket packet;
		packet.set_status_code(200);
		packet.set_status_message("OK");
		packet.set_res_header("Content-Type", "text/html");
		packet.set_content("<html><body><h1>Hello, World!</h1></body></html>");

		std::string response = packet.serializeResponse();
		std::string expected = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>";
		REQUIRE(response == expected);

		// Test serializing a response with no headers
		packet = HttpPacket();
		packet.set_status_code(404);
		packet.set_status_message("Not Found");
		packet.set_content("404 Not Found");

		response = packet.serializeResponse();
		expected = "HTTP/1.1 404 Not Found\n\n404 Not Found";
		REQUIRE(response == expected);

		// Test serializing a response with multiple headers
		packet = HttpPacket();
		packet.set_status_code(500);
		packet.set_status_message("Internal Server Error");
		packet.set_res_header("Content-Type", "text/plain");
		packet.set_res_header("Server", "WebServer/1.0");
		packet.set_content("500 Internal Server Error");

		response = packet.serializeResponse();
		expected = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\nServer: WebServer/1.0\n\n500 Internal Server Error";
		REQUIRE(response == expected);
	}
}
