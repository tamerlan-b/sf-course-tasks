#pragma once
#include "message.hpp"
#include "tcp_library.hpp"
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

class ChatServer
{
  private:
    TcpServer server;
    std::vector<Message> messages;
    std::unordered_map<std::string, std::string> users_table;
    std::string users_path;
    std::string messages_path;

    bool sign_up_handle(int socket, const std::string& msg, std::string& client_login);
    bool sign_in_handle(int socket, const std::string& msg, std::string& client_login);
    bool get_users_handle(int socket);
    bool get_history_handle(int socket, const std::string& client_login);
    bool send_msg_handle(int socket, const std::string& msg);
    void client_handler(int socket);
    void accept_clients();
    void load_users(const std::string& fname);
    static void save_user(const std::string& fname, const std::string& login, const std::string& pass_hash);

    // TODO: добавить проверку типов через SFINAE
    template <typename T> void load(const std::string& fname, std::vector<T>& objects)
    {
        std::fstream file(fname, std::ios::in);
        if (file)
        {
            while (!file.eof())
            {
                T obj;
                file >> obj;
                if (!obj.is_empty())
                {
                    objects.push_back(obj);
                }
            }
            file.close();
        }
        else
        {
            // cout << "Could not open file " << fname << " !" << '\n';
            return;
        }
    }

  public:
    ChatServer(std::string users_path, std::string messages_path);
    virtual ~ChatServer();
    void run();
};