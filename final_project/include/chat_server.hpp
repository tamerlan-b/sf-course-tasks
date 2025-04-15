#pragma once
#include "data_manager_interface.hpp"
#include "logger_interface.hpp"
#include "message.hpp"
#include "tcp_library.hpp"
#include "user.hpp"
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

class ChatServer
{
  private:
    TcpServer server;

    std::vector<Message> messages;
    mutable std::shared_mutex messages_mutex;

    std::vector<User> users;
    mutable std::shared_mutex users_mutex;

    std::unordered_map<int, int> users_sockets;
    mutable std::shared_mutex users_sockets_mutex;

    std::unique_ptr<IDataManager> data_manager;
    std::unique_ptr<skillfactory::ILogger> logger;

    bool sign_up_handle(int socket, const std::string& msg, int& user_id);
    bool sign_in_handle(int socket, const std::string& msg, int& user_id) const;
    bool get_users_handle(int socket) const;
    bool get_history_handle(int user_id, int socket) const;
    bool send_msg_handle(int user_id, int socket, const std::string& msg);
    void client_handler(int socket);
    void accept_clients();
    void wait_for_stop() const noexcept;
    int find_user(const std::string& login) const noexcept;
    int find_user(int id) const noexcept;

  public:
    ChatServer(std::unique_ptr<skillfactory::ILogger>& logger, std::unique_ptr<IDataManager>& data_manager);
    virtual ~ChatServer() = default;
    void run();
};