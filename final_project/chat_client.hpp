#pragma once
#include "chat_msgs.hpp"
#include "tcp_library.hpp"
#include <deque>
#include <string>
// namespace sf = skillfactory;

class ChatClient
{
  private:
    TcpClient client;

    // Очередь сообщений, полученных от сервера
    std::deque<std::string> server_msgs;

    bool get_users();
    bool get_history();
    bool send_msg(const std::string& login);
    bool sign_in(std::string& login);
    bool sign_up(std::string& login);
    std::string authorize();
    void chat_menu(const std::string& login);
    /**
     * @brief Слушает сообщения от сервера в отдельном потоке
     *
     */
    void listen_server();
    bool wait_for_response(std::string& response, skillfactory::ResponseStatus response_type, const int timeout = 1000);
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