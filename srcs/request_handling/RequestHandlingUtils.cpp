#include "RequestHandler.hpp"

// https://datatracker.ietf.org/doc/html/rfc9112#section-9.3
bool check_keep_alive(RequestPacket &packet)
{
    if (packet.getHeader("Connection") == "keep-alive")
        return true;
    if (packet.getHttpVersion() == "HTTP/1.0")
        return false;
    if (packet.getHeader("Connection") == "close")
        return false;

    return true;
}

// // this doesn't seem to work
// std::string find_longest_matching_route(const std::string &uri, const std::map<std::string, RouteConfig> &routes)
// {
//     std::string longest_matching_route;
//     for (const auto &route : routes)
//     {
//         // Check if the URI starts with the current route path
//         if (uri.compare(0, route.first.length(), route.first) == 0)
//         {
//             // Ensure that it's either an exact match or the next character is a boundary (like '/')
//             if (uri.length() == route.first.length() || uri[route.first.length()] == '/')
//             {
//                 // Update if this route is the longest match found so far
//                 if (route.first.length() > longest_matching_route.length())
//                 {
//                     longest_matching_route = route.first;
//                 }
//             }
//         }
//     }
//     return longest_matching_route;
// }

std::string find_longest_matching_route(const std::string &uri, const std::map<std::string, RouteConfig> &routes)
{
    std::string longest_matching_route;
    for (const auto &route : routes)
    {
        if (uri.compare(0, route.first.length(), route.first) == 0)
        {
            if (route.first.length() > longest_matching_route.length())
                longest_matching_route = route.first;
        }
    }
    return longest_matching_route;
}

bool has_file_ending(const std::string &uri)
{
    size_t dot_pos = uri.rfind('.');
    if (dot_pos == std::string::npos)
        return false; // No dot found, so no file ending

    return true;
}

std::string getContent_type(const std::string &file_ending)
{
    if (file_ending == "html" || file_ending == "htm")
        return "text/html";
    if (file_ending == "css")
        return "text/css";
    if (file_ending == "js")
        return "text/javascript";
    if (file_ending == "jpg" || file_ending == "jpeg")
        return "image/jpeg";
    if (file_ending == "png")
        return "image/png";
    if (file_ending == "gif")
        return "image/gif";
    if (file_ending == "ico")
        return "image/x-icon";
    if (file_ending == "svg")
        return "image/svg+xml";
    if (file_ending == "pdf")
        return "application/pdf";
    if (file_ending == "json")
        return "application/json";
    if (file_ending == "xml")
        return "application/xml";
    if (file_ending == "zip")
        return "application/zip";
    if (file_ending == "tar")
        return "application/x-tar";
    if (file_ending == "gz")
        return "application/gzip";
    if (file_ending == "mp3")
        return "audio/mpeg";
    if (file_ending == "wav")
        return "audio/wav";
    if (file_ending == "mp4")
        return "video/mp4";
    if (file_ending == "mpeg")
        return "video/mpeg";
    if (file_ending == "webm")
        return "video/webm";
    if (file_ending == "ogg")
        return "video/ogg";
    if (file_ending == "flac")
        return "audio/flac";
    if (file_ending == "txt")
        return "text/plain";
    if (file_ending == "csv")
        return "text/csv";
    if (file_ending == "rtf")
        return "application/rtf";
    if (file_ending == "doc")
        return "application/msword";
    if (file_ending == "docx")
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    if (file_ending == "xls")
        return "application/vnd.ms-excel";
    if (file_ending == "xlsx")
        return "application/vnd.open";
    if (file_ending == "ppt")
        return "application/vnd.ms-powerpoint";
    if (file_ending == "pptx")
        return "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    if (file_ending == "odt")
        return "application/vnd.oasis.opendocument.text";
    if (file_ending == "ods")
        return "application/vnd.oasis.opendocument.spreadsheet";
    if (file_ending == "odp")
        return "application/vnd.oasis.opendocument.presentation";
    if (file_ending == "odg")
        return "application/vnd.oasis.opendocument.graphics";
    if (file_ending == "odc")
        return "application/vnd.oasis.opendocument.chart";
    if (file_ending == "odb")
        return "application/vnd.oasis.opendocument.database";
    if (file_ending == "odf")
        return "application/vnd.oasis.opendocument.formula";
    if (file_ending == "odm")
        return "application/vnd.oasis.opendocument.text-master";
    if (file_ending == "ott")
        return "application/vnd.oasis.opendocument.text-template";
    if (file_ending == "ots")
        return "application/vnd.oasis.opendocument.spreadsheet-template";
    if (file_ending == "otp")
        return "application/vnd.oasis.opendocument.presentation-template";
    if (file_ending == "otg")
        return "application/vnd.oasis.opendocument.graphics-template";
    if (file_ending == "otc")
        return "application/vnd.oasis.opendocument.chart-template";
    if (file_ending == "otf")
        return "application/vnd.oasis.opendocument.formula-template";
    if (file_ending == "otm")
        return "application/vnd.oasis.opendocument.text-master-template";
    if (file_ending == "oth")
        return "application/vnd.oasis.opendocument.text-web";
    if (file_ending == "ott")
        return "application/vnd.oasis.opendocument.text-template";
    if (file_ending == "ots")
        return "application/vnd.oasis.opendocument.spreadsheet-template";
    if (file_ending == "otp")
        return "application/vnd.oasis.opendocument.presentation-template";
    if (file_ending == "otg")
        return "application/vnd.oasis.opendocument.graphics-template";
    if (file_ending == "otc")
        return "application/vnd.oasis.opendocument.chart-template";
    if (file_ending == "otf")
        return "application/vnd.oasis.opendocument.formula-template";
    return "application/octet-stream";
}

std::string get_pure_hostname(RequestPacket &packet)
{
    std::string host;
    host = packet.getHeader("Host");
    if (host.empty())
    {
        host = packet.getHeader("host");
    }
    size_t colonPos = host.find(':');
    if (colonPos != std::string::npos)
    {
        host.resize(colonPos);
    }
    return host;
}
