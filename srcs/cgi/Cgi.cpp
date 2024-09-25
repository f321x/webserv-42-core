#include "Cgi.hpp"
#include "logging.hpp"

Cgi::Cgi(const RequestPacket &request_packet, ResponsePacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
	// 1. check for the requested cgi script. access()
	ERROR("Header of request inside CGI constructor: " + request_packet.get_header("Host"));

	// 2. parse the relevant data for cgi into env and write body to the pipe
	// 3. fork and dup input and output of the child process
	// 4. execve the cgi script
	(void)_fds;
	(void)_pid;
	(void)_path_info;
	(void)_query_string;
	(void)_script_name;
	(void)request_packet;
	(void)response_packet;
	(void)config_pair;
}