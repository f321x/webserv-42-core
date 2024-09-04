#include "HttpPacket.hpp"

HttpPacket::HttpPacket() {
	_raw_packet = "";
	_method = GET;
	_uri = "";
	_http_version = "";
	_status_code = 0;
	_status_message = "";
	_request_headers = std::map<std::string, std::string>();
	_response_headers = std::map<std::string, std::string>();
}

HttpPacket::HttpPacket(const std::string &raw_packet) {
	_raw_packet = raw_packet;
	//Todo: parse raw_packet
	_method = GET;
	_uri = "";
	_http_version = "";
	_status_code = 0;
	_status_message = "";
	_request_headers = std::map<std::string, std::string>();
	_response_headers = std::map<std::string, std::string>();
}

HttpPacket::HttpPacket(const HttpPacket &other) {
	*this = other;
}

HttpPacket &HttpPacket::operator=(const HttpPacket &other) {
	_raw_packet = other._raw_packet;
	_method = other._method;
	_uri = other._uri;
	_http_version = other._http_version;
	_status_code = other._status_code;
	_status_message = other._status_message;
	_request_headers = other._request_headers;
	_response_headers = other._response_headers;
	return *this;
}

HttpPacket::~HttpPacket() {
}

std::string HttpPacket::serializeResponse() {
	return "";
}
