#include <catch2/catch_test_macros.hpp>
// #include "catch.hpp"
#include "ResponsePacket.hpp"

// Valid CGI response with both status and content-type
TEST_CASE("ResponsePacket valid CGI response with Status and Content-Type", "[ResponsePacket]")
{
	std::string valid_cgi_response = "Status: 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Success</body></html>";

	ResponsePacket response(valid_cgi_response);

	REQUIRE(response.getHeader("Content-Type") == "text/html");
	REQUIRE(response.getContent() == "<html><body>Success</body></html>");
	REQUIRE(response.serialize().find("HTTP/1.1 200 OK") != std::string::npos); // Ensure status line is correct
}

// Valid CGI response without Status header (should default to 200 OK)
TEST_CASE("ResponsePacket valid CGI response without Status", "[ResponsePacket]")
{
	std::string valid_cgi_response_no_status = "Content-Type: text/html\r\n\r\n<html><body>No Status</body></html>";

	ResponsePacket response(valid_cgi_response_no_status);

	REQUIRE(response.getHeader("Content-Type") == "text/html");
	REQUIRE(response.getContent() == "<html><body>No Status</body></html>");
	REQUIRE(response.serialize().find("HTTP/1.1 200 OK") != std::string::npos); // Should default to 200 OK
}

// Invalid CGI response without Content-Type header (should throw)
TEST_CASE("ResponsePacket invalid CGI response missing Content-Type", "[ResponsePacket]")
{
	std::string invalid_cgi_response_no_content_type = "Status: 200 OK\r\n\r\n<html><body>Missing Content-Type</body></html>";

	REQUIRE_THROWS_AS(ResponsePacket(invalid_cgi_response_no_content_type), std::runtime_error);
}

// Invalid CGI response without headers (should throw)
TEST_CASE("ResponsePacket invalid CGI response missing headers", "[ResponsePacket]")
{
	std::string invalid_cgi_response_no_headers = "<html><body>Missing Headers</body></html>";

	REQUIRE_THROWS_AS(ResponsePacket(invalid_cgi_response_no_headers), std::runtime_error);
}

// Valid CGI response with additional headers
TEST_CASE("ResponsePacket valid CGI response with additional headers", "[ResponsePacket]")
{
	std::string valid_cgi_response_additional_headers = "Status: 404 Not Found\r\nContent-Type: text/html\r\nX-Custom-Header: custom-value\r\n\r\n<html><body>Not Found</body></html>";

	ResponsePacket response(valid_cgi_response_additional_headers);

	REQUIRE(response.getHeader("Content-Type") == "text/html");
	REQUIRE(response.getHeader("X-Custom-Header") == "custom-value");
	REQUIRE(response.getContent() == "<html><body>Not Found</body></html>");
	REQUIRE(response.serialize().find("HTTP/1.1 404 Not Found") != std::string::npos); // Ensure custom status line
}

// Valid CGI response with LF-only line endings
TEST_CASE("ResponsePacket valid CGI response with LF-only line endings", "[ResponsePacket]")
{
	std::string valid_cgi_response_lf_only = "Status: 200 OK\nContent-Type: text/html\n\n<html><body>LF Line Endings</body></html>";

	ResponsePacket response(valid_cgi_response_lf_only);

	REQUIRE(response.getHeader("Content-Type") == "text/html");
	REQUIRE(response.getContent() == "<html><body>LF Line Endings</body></html>");
	REQUIRE(response.serialize().find("HTTP/1.1 200 OK") != std::string::npos);
}
