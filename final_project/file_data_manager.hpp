#pragma once

#include "data_manager_interface.hpp"
#include "message.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
namespace fs = std::filesystem;

class FileDataManager : public IDataManager
{
  private:
    std::string users_path;
    std::string msgs_path;

  public:
    FileDataManager(std::string users_path, std::string msgs_path)
        : users_path(std::move(users_path)), msgs_path(std::move(msgs_path))
    {
    }

    void save_user(const std::string& login, const std::string& pass_hash) override
    {
        std::fstream file(this->users_path, std::ios::out | std::ios::app);
        if (!file)
        {
            file = std::fstream(this->users_path, std::ios::out | std::ios::app | std::ios::trunc);
        }
        if (file)
        {
            // Оставляем права чтения и записи только владельцу файла
            fs::permissions(this->users_path, fs::perms::owner_exec | fs::perms::group_all | fs::perms::others_all,
                            fs::perm_options::remove);
            file << login << ' ' << pass_hash << '\n';
            file.close();
        }
        else
        {
            std::cout << "Could not open file " << this->users_path << " !" << '\n';
            return;
        }
    }

    void load_users(std::unordered_map<std::string, std::string>& users) const override
    {
        std::fstream file(this->users_path, std::ios::in);
        if (file)
        {
            std::string user_login;
            std::string user_pass_hash;
            while (!file.eof())
            {
                file >> user_login;
                file >> user_pass_hash;
                if (!user_login.empty() && !user_pass_hash.empty())
                {
                    users.emplace(std::move(user_login), std::move(user_pass_hash));
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

    void save_msg(const Message& msg) override
    {
        std::fstream file(this->msgs_path, std::ios::out | std::ios::app);
        if (!file)
        {
            file = std::fstream(this->msgs_path, std::ios::out | std::ios::app | std::ios::trunc);
        }
        if (file)
        {
            // Оставляем права чтения и записи только владельцу файла
            fs::permissions(this->msgs_path, fs::perms::owner_exec | fs::perms::group_all | fs::perms::others_all,
                            fs::perm_options::remove);
            file << msg << '\n';
            file.close();
        }
        else
        {
            std::cout << "Could not open file " << this->msgs_path << " !" << '\n';
            return;
        }
    }

    void load_msgs(std::vector<Message>& msgs) const override
    {
        std::fstream file(this->msgs_path, std::ios::in);
        if (file)
        {
            while (!file.eof())
            {
                Message msg;
                file >> msg;
                if (!msg.is_empty())
                {
                    msgs.push_back(msg);
                }
            }
            file.close();
        }
        else
        {
            // cout << "Could not open file " << this->msgs_path << " !" << '\n';
            return;
        }
    }
};