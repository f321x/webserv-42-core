#include "RequestHandler.hpp"

std::unique_ptr<HttpPacket> handle_get(const HttpPacket &request_packet, std::unique_ptr<HttpPacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    // Get the requested file path
    std::string file_path;
    if (request_packet.get_uri().back() == '/')
        file_path = config_pair.second.getRoot() + request_packet.get_uri() + config_pair.second.getDefaultFile();
    else
        file_path = config_pair.second.getRoot() + request_packet.get_uri();

    auto file = load_file_with_cache(file_path);
    if (!file.has_value())
        return not_found();

    return ok_with_content(file.value(), std::move(response_packet));
}

std::optional<File> load_file_with_cache(const std::string &filepath)
{
    // we have lots of memory, lets use it
    static std::map<std::string, File> cache;
    if (cache.find(filepath) != cache.end())
        return cache[filepath];

    // Open the file
    std::ifstream fs(filepath);
    if (!fs.is_open())
    {
        WARN("Failed to open file: " + filepath);
        return std::nullopt;
    }

    // Read the file contents into a string
    std::stringstream buffer;
    buffer << fs.rdbuf();
    std::string file_contents = buffer.str();

    // Close the file
    fs.close();

    File file;
    file.content = file_contents;
    file.file_ending = filepath.substr(filepath.find_last_of('.') + 1);

    // Cache the file contents
    cache[filepath] = file;

    return file;
}