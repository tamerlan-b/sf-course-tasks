#pragma once
#include "message.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class IDataManager
{
  public:
    virtual void save_user(const std::string& login, const std::string& pass_hash) = 0;
    virtual void load_users(std::unordered_map<std::string, std::string>& users) const = 0;
    virtual void save_msg(const Message& msg) = 0;
    virtual void load_msgs(std::vector<Message>& msgs) const = 0;
    virtual ~IDataManager(){};
};