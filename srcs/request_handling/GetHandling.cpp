#include "RequestHandler.hpp"

std::unique_ptr<ResponsePacket> handle_get(const RequestPacket &request_packet, std::unique_ptr<ResponsePacket> response_packet, const std::pair<ServerConfig, RouteConfig> &config_pair)
{
    UriInfo uri_info = get_uri_info(request_packet.get_uri(), config_pair.second);
    switch (uri_info.type)
    {
    case UriType::FILE_REQUEST:
    {
        if (config_pair.second.isCgi())
        {
            auto cgi = Cgi(request_packet, *response_packet, config_pair);
            // cgi.execute();
            // return
        }
        else
        {
            std::optional<File> file = load_file_with_cache(uri_info.path);
            if (!file)
                return not_found(load_error_page(404, config_pair.first));
            return ok_with_content(file.value(), std::move(response_packet));
        }
    }
    case UriType::AUTOINDEX:
    {
        std::string index = get_autoindex(uri_info.path, request_packet.get_uri());
        if (index.empty())
            return not_found(load_error_page(404, config_pair.first));
        return autoindex_response(index, std::move(response_packet));
    }
    case UriType::REDIRECT:
        return redirect(uri_info.path); // maybe wrong
    case UriType::INVALID:
        return not_found(load_error_page(404, config_pair.first));
    }
    return not_found(load_error_page(404, config_pair.first));
}

std::string load_error_page(int status_code, const ServerConfig &server)
{
    std::map<int, std::string> error_pages = server.getErrorPages();
    if (error_pages.find(status_code) != error_pages.end())
    {
        std::optional<File> file = load_file_with_cache(error_pages[status_code]);
        if (file)
            return file.value().content;
    }
    return "Not found 404";
}

std::string get_autoindex(std::string path, const std::string &uri)
{
    if (has_file_ending(path))
    {
        auto index_file = load_file_with_cache(path); // try to open index if available
        if (index_file.has_value())
            return index_file.value().content;

        size_t last_slash = path.find_last_of('/'); // remove file ending
        if (last_slash != std::string::npos)
            path = path.substr(0, last_slash);
    }
    std::stringstream index;
    index << "<!DOCTYPE html>\n"
          << "<html lang=\"en\">\n"
          << "<head>\n"
          << "    <meta charset=\"UTF-8\">\n"
          << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
          << "    <title>Index of " << uri << "</title>\n"
          << "    <style>\n"
          << "        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }\n"
          << "        h1 { color: #333; }\n"
          << "        table { border-collapse: collapse; width: 100%; }\n"
          << "        th, td { text-align: left; padding: 8px; border-bottom: 1px solid #ddd; }\n"
          << "        tr:hover { background-color: #f5f5f5; }\n"
          << "        a { text-decoration: none; color: #0066cc; }\n"
          << "        .size { text-align: right; }\n"
          << "    </style>\n"
          << "</head>\n"
          << "<body>\n"
          << "    <h1>Index of " << uri << "</h1>\n"
          << "    <table>\n"
          << "        <tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>\n";

    DEBUG("Autoindexing directory: " + path);
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            std::string filename = ent->d_name;
            std::string fullpath = path + "/" + filename;
            struct stat st;

            if (stat(fullpath.c_str(), &st) == 0)
            {
                char time_str[50];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&st.st_mtime));

                index << "        <tr>\n"
                      << "            <td><a href=\"" << filename << "\">" << filename << "</a></td>\n"
                      << "            <td>" << time_str << "</td>\n"
                      << "            <td class=\"size\">";

                if (S_ISDIR(st.st_mode))
                    index << "-";
                else
                    index << st.st_size << " bytes";

                index << "</td>\n        </tr>\n";
            }
        }
        closedir(dir);
    }
    else
    {
        DEBUG("Failed to open directory: " + path);
        return "";
    }

    index << "    </table>\n"
          << "</body>\n"
          << "</html>";

    return index.str();
}

std::optional<File> load_file_with_cache(const std::string &filepath)
{
    // we have lots of memory, lets use it
    static std::unordered_map<std::string, File> cache;
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
    if (file.content.size() < 1000000) // 1MB
        cache[filepath] = file;

    return file;
}

UriInfo get_uri_info(const std::string &uri, const RouteConfig &route_config)
{
    UriInfo uri_info;
    if (!route_config.getRedirection().empty())
    {
        uri_info.path = route_config.getRedirection();
        uri_info.type = UriType::REDIRECT;
    }
    else if (has_file_ending(uri)) // is a file GET request
    {
        uri_info.path = route_config.getRoot() + uri;
        uri_info.type = UriType::FILE_REQUEST;
    }
    else if (uri.back() == '/') // is a directory request
    {
        if (route_config.isAutoindex())
        {
            uri_info.path = route_config.getRoot() + uri + route_config.getDefaultFile();
            uri_info.type = UriType::AUTOINDEX;
        }
        else if (!route_config.getDefaultFile().empty())
        {
            uri_info.path = route_config.getRoot() + uri + route_config.getDefaultFile();
            uri_info.type = UriType::FILE_REQUEST;
        }
        else
            uri_info.type = UriType::INVALID;
    }
    else
    {
        uri_info.path = uri + "/";
        uri_info.type = UriType::REDIRECT;
    }
    return uri_info;
}
