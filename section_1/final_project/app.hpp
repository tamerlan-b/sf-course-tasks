#pragma once
#include <vector>
#include <string>
#include "user.hpp"
#include "message.hpp"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

class App
{
    public:
        App(std::string users_path, std::string messages_path);
        virtual ~App() = default;
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

        // TODO: добавить проверку типов через SFINAE
        template<typename T>
        void save(const std::string& fname, const T& obj) const
        {
            std::fstream file(fname, std::ios::out | std::ios::app);
            if (!file)
                file = std::fstream(fname, std::ios::out | std::ios::app | std::ios::trunc);
            if (file) {
                // Оставляем права чтения и записи только владельцу файла
                fs::permissions(fname, fs::perms::owner_exec | fs::perms::group_all | fs::perms::others_all, fs::perm_options::remove);
                file << obj << '\n';
                file.close();
            }
            else
            {
                std::cout << "Could not open file " << fname << " !" << '\n';
                return;
            }
        }

        // TODO: добавить проверку типов через SFINAE
        template<typename T>
        void load(const std::string& fname, std::vector<T>& objects)
        {
            std::fstream file(fname, std::ios::in);
            if (file) {
                while(!file.eof())
                {
                    T obj;
                    file >> obj;
                    if(!obj.is_empty())
                        objects.push_back(obj);
                }
                file.close();
            }
            else
            {
                // cout << "Could not open file " << fname << " !" << '\n';
                return;
            }
        }
};