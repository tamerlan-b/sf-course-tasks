#pragma once
#include "data_manager_interface.hpp"
#include "logger_interface.hpp"
#include "message.hpp"
#include "tcp_library.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ChatServer
{
  private:
    TcpServer server;
    std::vector<Message> messages;
    std::unordered_map<std::string, std::string> users_table;
    std::unordered_map<std::string, int> users_sockets;
    std::unique_ptr<IDataManager> data_manager;
    std::unique_ptr<skillfactory::ILogger> logger;

    bool sign_up_handle(int socket, const std::string& msg, std::string& client_login);
    bool sign_in_handle(int socket, const std::string& msg, std::string& client_login);
    bool get_users_handle(int socket);
    bool get_history_handle(int socket, const std::string& client_login);
    bool send_msg_handle(int socket, const std::string& msg);
    void client_handler(int socket);
    void accept_clients();
    void wait_for_stop();

  public:
    ChatServer(std::unique_ptr<skillfactory::ILogger>& logger, std::unique_ptr<IDataManager>& data_manager);
    virtual ~ChatServer() = default;
    void run();
};