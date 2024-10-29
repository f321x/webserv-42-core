#pragma once

#include "TcpSocket.hpp"
#include "WebServerConfig.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"

#include <unordered_set>
#include <limits.h>
#include <optional>

class HttpSocket
{
public:
    // bind socket (stores all configs suitable for the port)
    HttpSocket(const HostPortPair &host, const WebServerConfig &config);
    // client socket (takes accepted socket and configs)
    HttpSocket(std::unique_ptr<TcpSocket> socket, std::shared_ptr<std::vector<ServerConfig>> available_configs);
    ~HttpSocket();
    HttpSocket(const HttpSocket &other);
    HttpSocket &operator=(const HttpSocket &other);

    const bool is_bind_socket = false;

    // functions
    std::unique_ptr<HttpSocket> accept_connection();
    pollfd new_pfd() const;
    void handle_client_data();
    std::chrono::steady_clock::time_point last_activity() const;
    bool write_client_response();

    std::optional<std::unique_ptr<ResponsePacket>> response;

private:
    // private variables
    std::unique_ptr<TcpSocket>
        _socket;
    std::shared_ptr<std::vector<ServerConfig>> _available_configs;
    std::chrono::steady_clock::time_point _last_activity;

    // private functions
    sockaddr_in _compose_sockaddr(const std::string &addr, int port);
    std::unique_ptr<TcpSocket> _create_bind_socket(const sockaddr_in &address);
    int _smallest_max_body_size() const;
    std::unique_ptr<RequestPacket> request;
};

class HttpSocketError : public std::exception
{
public:
    explicit HttpSocketError(const std::string &message) : msg_(message) {}
    virtual const char *what() const noexcept override
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

class IsBindSocketErr : public HttpSocketError
{
public:
    explicit IsBindSocketErr(const std::string &message) : HttpSocketError(message) {}
};

class ReadingFailedErr : public HttpSocketError
{
public:
    explicit ReadingFailedErr(const std::string &message) : HttpSocketError(message) {}
};

class WritingFailedErr : public HttpSocketError
{
public:
    explicit WritingFailedErr(const std::string &message) : HttpSocketError(message) {}
};

class IsFinalResponse : public HttpSocketError
{
public:
    explicit IsFinalResponse(const std::string &message) : HttpSocketError(message) {}
};

class EmptyReadErr : HttpSocketError
{
public:
    explicit EmptyReadErr(const std::string &message) : HttpSocketError(message) {}
};
