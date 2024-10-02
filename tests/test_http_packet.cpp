// #include <catch2/catch_test_macros.hpp>

// #include "RequestPacket.hpp"
// #include "ResponsePacket.hpp"

// TEST_CASE("HttpPacket parsing")
// {
// 	SECTION("Request parsing")
// 	{
// 		// Test parsing a simple GET request
// 		std::string request = "GET /index.html HTTP/1.1\r\nHost: localhost:4242\r\n\r\n";
// 		RequestPacket packet(request);

// 		REQUIRE(packet.getMethod() == GET);
// 		REQUIRE(packet.getUri() == "/index.html");
// 		REQUIRE(packet.getHttpVersion() == "HTTP/1.1");
// 		REQUIRE(packet.get_req_header("Host") == "localhost:4242");

// 		// Test parsing no headers
// 		request = "GET / HTTP/1.1\r\n\r\n";
// 		packet = RequestPacket(request);
// 		REQUIRE(packet.getMethod() == GET);
// 		REQUIRE(packet.getUri() == "/");
// 		REQUIRE(packet.getHttpVersion() == "HTTP/1.1");
// 		REQUIRE(packet.get_req_headers().size() == 0);

// 		// Test parsing multiple headers
// 		request = "POST / HTTP/1.1\r\nHost: localhost:4242\r\nUser-Agent: Mozilla/5.0\r\n\r\n";
// 		packet = RequestPacket(request);
// 		REQUIRE(packet.getMethod() == POST);
// 		REQUIRE(packet.getUri() == "/");
// 		REQUIRE(packet.getHttpVersion() == "HTTP/1.1");
// 		REQUIRE(packet.get_req_header("Host") == "localhost:4242");
// 		REQUIRE(packet.get_req_header("User-Agent") == "Mozilla/5.0");

// 		// Test invalid Method
// 		request = "INVALID_METHOD / HTTP/1.1\r\n\r\n";
// 		REQUIRE_THROWS_AS(RequestPacket(request), RequestPacket::UnknownMethodException);

// 		// Test invalid request line
// 		request = "GET / ";
// 		REQUIRE_THROWS_AS(RequestPacket(request), RequestPacket::InvalidPacketException);
// 	}

// 	SECTION("Response serialization")
// 	{
// 		// Test serializing a simple response
// 		ResponsePacket packet;
// 		packet.set_status_code(200);
// 		packet.set_status_message("OK");
// 		packet.set_res_header("Content-Type", "text/html");
// 		packet.set_content("<html><body><h1>Hello, World!</h1></body></html>");

// 		std::string response = packet.serialize();
// 		std::string expected = "HTTP/1.1 200 OK\nContent-Length: 48\nContent-Type: text/html\n\n<html><body><h1>Hello, World!</h1></body></html>";
// 		REQUIRE(response == expected);

// 		// Test serializing a response with no headers
// 		packet = ResponsePacket();
// 		packet.set_status_code(404);
// 		packet.set_status_message("Not Found");
// 		packet.set_content("404 Not Found");

// 		response = packet.serialize();
// 		expected = "HTTP/1.1 404 Not Found\n\n404 Not Found";
// 		REQUIRE(response == expected);

// 		// Test serializing a response with multiple headers
// 		packet = ResponsePacket();
// 		packet.set_status_code(500);
// 		packet.set_status_message("Internal Server Error");
// 		packet.set_res_header("Content-Type", "text/plain");
// 		packet.set_res_header("Server", "WebServer/1.0");
// 		packet.set_content("500 Internal Server Error");

// 		response = packet.serialize();
// 		expected = "HTTP/1.1 500 Internal Server Error\nContent-Type: text/plain\nServer: WebServer/1.0\n\n500 Internal Server Error";
// 		REQUIRE(response == expected);
// 	}
// }
