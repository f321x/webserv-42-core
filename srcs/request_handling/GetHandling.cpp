#include "RequestHandler.hpp"

void handle_get(const HttpPacket &request_packet, HttpPacket &response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    (void)request_packet;
    (void)response_packet;
    (void)config_pair;
}

std::string load_file(const std::string &filepath)
{
    // Open the file
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        WARN("Failed to open file: " + filepath);
        return "";
    }

    // Read the file contents into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();

    // Close the file
    file.close();

    return file_contents;
}