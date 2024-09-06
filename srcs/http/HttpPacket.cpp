#include "HttpPacket.hpp"
#include "Utils.hpp"

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
	parseRawPacket();
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

std::string HttpPacket::get_http_version() const {
	return _http_version;
}

std::string HttpPacket::get_uri() const {
	return _uri;
}

Method HttpPacket::get_method() const {
	return _method;
}

std::string HttpPacket::get_req_header(const std::string &key) const {
	std::map<std::string, std::string>::const_iterator it = _request_headers.find(key);
	if (it == _request_headers.end()) {
		return "";
	}
	return it->second;
}

std::map<std::string, std::string> HttpPacket::get_req_headers() const {
	return _request_headers;
}

void HttpPacket::set_status_code(uint status_code) {
	_status_code = status_code;
}

void HttpPacket::set_status_message(const std::string status_message) {
	_status_message = status_message;
}

void HttpPacket::set_res_header(const std::string key, const std::string value) {
	_response_headers.insert(std::make_pair(key, value));
}

std::string HttpPacket::serializeResponse() {
	return "";
}

void HttpPacket::parseRawPacket() {
	std::vector<std::string> lines = split(_raw_packet, '\n');

	for (size_t lInd = 0; lInd < lines.size(); lInd++) {
		if (lInd == 0) {
			std::vector<std::string> tokens = split(lines[lInd], ' ');
			// Request line
			if (tokens.size() != 3) {
				// Invalid request line
			}

			if (tokens[0] == "GET") {
				_method = GET;
			} else if (tokens[0] == "POST") {
				_method = POST;
			} else if (tokens[0] == "DELETE") {
				_method = DELETE;
			} else {
				// Throw Error
			}

			_uri = tokens[1];
			_http_version = tokens[2];
			continue;
		}
		if (lines[lInd].empty()) {
			continue;
		}

		std::vector<std::string> tokens = split(lines[lInd], ':');
		if (tokens.size() != 2) {
			// Invalid header
		}

		_request_headers.insert(std::make_pair(tokens[0], tokens[1]));
	}
}
