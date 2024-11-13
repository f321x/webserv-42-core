#include "RequestHandler.hpp"
#include <filesystem>

std::shared_ptr<ResponsePacket> handle_delete(const RequestPacket &request_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    UriInfo uri_info = getUri_info(request_packet.getUri(), config_pair.second);
    std::string full_path = config_pair.second.getRoot() + request_packet.getUri();

    TRACE("URI info: " + uri_info.path);
    TRACE("Full path: " + full_path);

    struct stat path_stat;
    if (stat(full_path.c_str(), &path_stat) != 0)
    {
        TRACE("File not found: " + full_path);
        return not_found(load_error_page(404, config_pair.first));
    }

    // Check if we have write permission
    if (access(full_path.c_str(), W_OK) != 0)
    {
        TRACE("No write permission for: " + full_path);
        return forbidden();
    }

    bool is_directory = S_ISDIR(path_stat.st_mode);
    std::string uri = request_packet.getUri();
    bool ends_with_slash = !uri.empty() && uri.back() == '/';

    // URI ends with slash but target is a file
    if (ends_with_slash && !is_directory)
    {
        TRACE("URI ends with slash but target is a file");
        return conflict();
    }

    // URI doesn't end with slash but target is a directory
    if (!ends_with_slash && is_directory)
    {
        TRACE("URI doesn't end with slash but target is a directory");
        return conflict();
    }

    // Attempt to delete the file/directory
    int result;
    if (is_directory)
    {
        TRACE("Deleting directory: " + full_path);
        try
        {
            if (!std::filesystem::is_empty(full_path))
            {
                TRACE("Directory is not empty: " + full_path);
                return conflict();
            }
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            TRACE("Error checking if directory is empty: " + std::string(e.what()));
            return internal_server_error();
        }
        result = rmdir(full_path.c_str());
    }
    else
    {
        TRACE("Deleting file: " + full_path);
        result = unlink(full_path.c_str());
    }

    if (result != 0)
    {
        TRACE("Failed to delete: " + full_path);
        return internal_server_error();
    }

    TRACE("Successfully deleted: " + full_path);
    return no_content();
}
