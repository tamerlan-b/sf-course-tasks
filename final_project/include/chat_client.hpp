#pragma once
#include "chat_msgs.hpp"
#include "tcp_library.hpp"
#include <deque>
#include <string>
#include <vector>
// namespace sf = skillfactory;

class ChatClient
{
  public:
    struct SimpleUser
    {
        int id;
        std::string login;
    };

  private:
    TcpClient client;

    // Очередь сообщений, полученных от сервера
    std::deque<std::string> server_msgs;

    std::vector<SimpleUser> users;

    bool get_users();
    bool get_history();
    bool send_msg(const std::string& login);
    bool sign_in(std::string& login);
    bool sign_up(std::string& login);
    std::string authorize();
    void remove_new_messages();
    void render_chat_menu();
    void chat_menu(const std::string& login);

    [[nodiscard]] int get_user_by_id(int id) const;
    /**
     * @brief Слушает сообщения от сервера в отдельном потоке
     *
     */
    void listen_server();
    bool wait_for_response(std::string& response, skillfactory::MsgType msg_type, const int timeout = 1000);

    enum Cmds
    {
        SIGN_IN = 1,
        SIGN_UP = 2,
        EXIT = 3
    };

    enum ChatCmds
    {
        SHOW_CHAT = 1,
        SHOW_USERS = 2,
        WRITE_MSG = 3,
        CHAT_EXIT = 4
    };

  public:
    ChatClient();
    virtual ~ChatClient() = default;
    void run();
};