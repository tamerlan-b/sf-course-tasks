#pragma once
#include "data_manager_interface.hpp"
#include "logger_interface.hpp"
#include "message.hpp"
#include "tcp_library.hpp"
#include "user.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ChatServer
{
  private:
    TcpServer server;
    std::vector<Message> messages;
    std::vector<User> users;

    std::unordered_map<int, int> users_sockets;
    std::unique_ptr<IDataManager> data_manager;
    std::unique_ptr<skillfactory::ILogger> logger;

    bool sign_up_handle(int socket, const std::string& msg, int& user_id);
    bool sign_in_handle(int socket, const std::string& msg, int& user_id);
    bool get_users_handle(int socket);
    bool get_history_handle(int user_id, int socket);
    bool send_msg_handle(int user_id, int socket, const std::string& msg);
    void client_handler(int socket);
    void accept_clients();
    void wait_for_stop();
    int find_user(const std::string& login) const noexcept;
    int find_user(int id) const noexcept;

  public:
    ChatServer(std::unique_ptr<skillfactory::ILogger>& logger, std::unique_ptr<IDataManager>& data_manager);
    virtual ~ChatServer() = default;
    void run();
};