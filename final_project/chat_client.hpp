#pragma once
#include "tcp_library.hpp"
#include <string>

class ChatClient
{
  private:
    TcpClient client;

    bool get_users();
    bool get_history();
    bool send_msg(const std::string& login);
    bool sign_in(std::string& login);
    bool sign_up(std::string& login);
    std::string authorize();
    void chat_menu(const std::string& login);

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
    virtual ~ChatClient();
    void run();
};