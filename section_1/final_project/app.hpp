#pragma once
#include <vector>
#include <string>
#include "user.hpp"
#include "message.hpp"

class App
{
    public:
        App(std::string users_path, std::string messages_path);
        ~App(){}
        void run();
        int create_user();
        bool is_login_available(std::string& login);
        int authorize();
        int sign_in();
        void chat_menu(const int user_id);
        void show_users(const bool add_all=false) const noexcept;
        void write_msg(const int user_id);
        void show_messages(int user_id) const noexcept;
    private:
        std::vector<Message> messages;
        std::vector<User> users;
        std::string users_path;
        std::string messages_path;
        void load_users();
        void save_user(const User& user) const;
        void load_messages();
        void save_message(const Message& msg) const;
};