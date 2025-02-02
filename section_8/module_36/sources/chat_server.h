#pragma once
#include "data_manager_interface.h"
#include "logger_interface.h"
#include "message.h"
#include "tcp_library.h"
#include "user.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include "custom_event.h"


class ChatServer
{
  private:

    enum UserStatus
    {
        NOT_CONNECTED=0,
        CONNECTED=1,
        DISCONNECTED=2,
        BANNED=3
    };
    TcpServer server;
    std::vector<Message> messages;
    std::vector<User> users;
    std::unordered_map<int, std::thread> socket_threads;

    std::unordered_map<int, int> users_statuses;

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
    bool set_user_status(int user_id, UserStatus status);

  public:
    ChatServer(std::unique_ptr<skillfactory::ILogger>& logger, std::unique_ptr<IDataManager>& data_manager);
    virtual ~ChatServer();
    void get_users(std::vector<User>& users) const noexcept;
    void get_messages(std::vector<Message>& msgs) const noexcept;

    bool disconnect_user(int user_id);
    bool ban_user(int user_id);
    void run();

    // UpdateUsersEvent update_user_event;

    Event<const std::vector<User>&> update_users_event;
    Event<const std::vector<Message>&> update_messages_event;
};
