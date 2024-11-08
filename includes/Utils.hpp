#pragma once

#include <cstring>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>

std::vector<std::string> split(const std::string &str, const char delim);
std::string trim(const std::string &str);
void remove_content_from_packet(std::string &packet, const std::string &content);
std::string &toLowerCase(std::string &str);

struct HostPortPair
{
    std::string host;
    uint16_t port;

    bool operator==(const HostPortPair &other) const
    {
        return host == other.host && port == other.port;
    }
};

struct HostPortPairHash
{
    std::size_t operator()(const HostPortPair &hp) const
    {
        auto hash1 = std::hash<std::string>{}(hp.host);
        auto hash2 = std::hash<uint16_t>{}(hp.port);
        return hash1 ^ (hash2 << 1);
    }
};

enum Method
{
    GET,
    POST,
    DELETE
};
