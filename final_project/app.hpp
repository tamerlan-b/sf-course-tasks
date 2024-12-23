#pragma once
#include "message.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
namespace fs = std::filesystem;

class App
{
  public:
    App(std::string users_path, std::string messages_path);
    virtual ~App() = default;
    void run();
    std::string create_user();
    bool is_login_available(std::string& login);
    std::string authorize();
    std::string sign_in();
    void chat_menu(const std::string& login);
    void show_users(bool add_all = false) const noexcept;
    void write_msg(const std::string& login);
    void show_messages(const std::string& login) const noexcept;

  private:
    std::vector<Message> messages;
    std::unordered_map<std::string, std::string> users_table;
    std::string users_path;
    std::string messages_path;

    // TODO: добавить проверку типов через SFINAE
    template <typename T> void save(const std::string& fname, const T& obj) const
    {
        std::fstream file(fname, std::ios::out | std::ios::app);
        if (!file)
        {
            file = std::fstream(fname, std::ios::out | std::ios::app | std::ios::trunc);
        }
        if (file)
        {
            // Оставляем права чтения и записи только владельцу файла
            fs::permissions(fname, fs::perms::owner_exec | fs::perms::group_all | fs::perms::others_all,
                            fs::perm_options::remove);
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

    void load_users(const std::string& fname);

    static void save_user(const std::string& fname, const std::string& login, const std::string& pass_hash);
};