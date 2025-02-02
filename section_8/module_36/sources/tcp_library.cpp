#include "tcp_library.h"
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MESSAGE_LENGTH 1024 // Максимальный размер буфера для данных

bool TcpServer::create_socket()
{
    this->socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    return this->socket_file_descriptor != -1;
}

bool TcpServer::bind_socket(uint16_t port)
{
    //
    this->serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим номер порта для связи
    this->serveraddress.sin_port = htons(port);
    // Используем IPv4
    this->serveraddress.sin_family = AF_INET;
    // Привяжем сокет
    this->bind_status =
        bind(this->socket_file_descriptor, (struct sockaddr*)&this->serveraddress, sizeof(this->serveraddress));

    return this->bind_status != -1;
}

[[nodiscard]] bool TcpServer::enable_listening(int num_connections) const
{
    int connection_status = listen(this->socket_file_descriptor, num_connections);
    return connection_status != -1;
}

std::optional<int> TcpServer::wait_for_connection()
{
    struct sockaddr_in client;
    socklen_t length = sizeof(client);
    int conn = accept(this->socket_file_descriptor, (struct sockaddr*)&client, &length);
    this->connection = conn;
    if (conn != -1)
    {
        return conn;
    }
    return {};
}

bool TcpServer::read_msg(int socket, std::string& msg)
{
    char message[MESSAGE_LENGTH];
    bzero(message, MESSAGE_LENGTH);
    read(socket, message, sizeof(message));
    msg = std::string(message);
    return strncmp("end", message, 3) != 0;
}

[[nodiscard]] bool TcpServer::send_msg(int socket, const std::string& msg)
{
    // TODO: передавать сообщение любой длины (а не только MESSAGE_LENGTH)
    char message[MESSAGE_LENGTH];
    strncpy(message, msg.c_str(), MESSAGE_LENGTH);
    ssize_t bytes = write(socket, message, sizeof(message));
    // Если передали >= 0  байт, значит пересылка прошла успешно
    return bytes >= 0;
}

TcpServer::~TcpServer()
{
    if (this->socket_file_descriptor != -1)
    {
        close(this->socket_file_descriptor);
    }
}

bool TcpClient::create_socket()
{
    this->socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    return this->socket_file_descriptor != -1;
}

bool TcpClient::connect_server(uint16_t port)
{
    // Установим адрес сервера
    this->serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Зададим номер порта
    this->serveraddress.sin_port = htons(port);
    // Используем IPv4
    this->serveraddress.sin_family = AF_INET;
    // Установим соединение с сервером
    int connection = connect(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
    return connection != -1;
}

bool TcpClient::read_msg(std::string& msg) const
{
    char message[MESSAGE_LENGTH];
    bzero(message, MESSAGE_LENGTH);
    read(this->socket_file_descriptor, message, sizeof(message));
    msg = std::string(message);
    return strncmp("end", message, 3) != 0;
}

[[nodiscard]] bool TcpClient::send_msg(const std::string& msg) const
{
    // TODO: передавать сообщение любой длины ( а не только
    // MESSAGE_LENGTH)
    char message[MESSAGE_LENGTH];
    strncpy(message, msg.c_str(), MESSAGE_LENGTH);
    ssize_t bytes = write(this->socket_file_descriptor, message, sizeof(message));
    // Если передали >= 0  байт, значит пересылка прошла успешно
    return bytes >= 0;
}

TcpClient::~TcpClient()
{
    if (this->socket_file_descriptor != -1)
    {
        close(this->socket_file_descriptor);
    }
}
