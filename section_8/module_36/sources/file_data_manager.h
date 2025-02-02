#pragma once

#include "data_manager_interface.h"
#include "message.h"
#include "user.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
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
        auto create_folder_if_not_exist = [](const std::string& path)
        {
            auto path_fs = fs::path(path);
            if (!fs::exists(path_fs))
            {
                if (path_fs.has_parent_path())
                {
                    fs::create_directory(path_fs.parent_path());
                }
            }
        };

        create_folder_if_not_exist(this->users_path);
        create_folder_if_not_exist(this->msgs_path);
    }

    void save_user(User& user) override
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
            file << user << '\n';
            file.close();
        }
        else
        {
            std::cout << "Could not open file " << this->users_path << " !" << '\n';
            return;
        }
    }

    void load_users(std::vector<User>& users) const override
    {
        std::fstream file(this->users_path, std::ios::in);
        if (file)
        {
            while (!file.eof())
            {
                User user;
                file >> user;
                if (!user.is_empty())
                {
                    users.push_back(user);
                }
            }
            file.close();
        }
        else
        {
            // cout << "Could not open file " << this->users_path << " !" << '\n';
            return;
        }
    }

    void save_msg(Message& msg) override
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
