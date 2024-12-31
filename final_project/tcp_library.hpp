#pragma once

#include <arpa/inet.h>
#include <optional>
#include <string>

class TcpServer
{
  private:
    int socket_file_descriptor;
    int connection;
    int bind_status;

    struct sockaddr_in serveraddress;

  public:
    TcpServer() = default;

    bool create_socket();

    bool bind_socket(uint16_t port = 7777);

    [[nodiscard]] bool enable_listening(int num_connections = 5) const;

    std::optional<int> wait_for_connection();

    static bool read_msg(int socket, std::string& msg);

    [[nodiscard]] static bool send_msg(int socket, const std::string& msg);

    virtual ~TcpServer();
};

class TcpClient
{
  private:
    int socket_file_descriptor;
    struct sockaddr_in serveraddress;

  public:
    TcpClient() = default;

    bool create_socket();

    bool connect_server(uint16_t port = 7777);

    bool read_msg(std::string& msg) const;

    [[nodiscard]] bool send_msg(const std::string& msg) const;

    virtual ~TcpClient();
};