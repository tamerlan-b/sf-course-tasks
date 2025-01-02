#pragma once
#include "data_manager_interface.hpp"
#include "message.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace fs = std::filesystem;

class App
{
  public:
    App(std::shared_ptr<IDataManager> data_manager);
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
    std::shared_ptr<IDataManager> data_manager;
    std::vector<Message> messages;
    std::unordered_map<std::string, std::string> users_table;
};