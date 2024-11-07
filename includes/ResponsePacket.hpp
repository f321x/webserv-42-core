#pragma once

#include <sstream>
#include <atomic>
#include "BasePacket.hpp"

class ResponsePacket : public BasePacket
{
public:
	ResponsePacket();
	ResponsePacket(const std::string &cgi_response);
	ResponsePacket(const ResponsePacket &other);
	ResponsePacket &operator=(const ResponsePacket &other);
	~ResponsePacket();

	void set_status_code(uint status_code);
	void set_status_message(const std::string status_message);
	void set_final_response();
	bool is_final_response() const;
	bool getResponseReady() const;

	std::string serialize();

private:
	uint _status_code;
	std::string _status_message;
	bool _final_response;
	std::atomic<bool> _response_ready{false};
};
