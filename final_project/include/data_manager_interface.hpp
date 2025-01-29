#pragma once
#include "message.hpp"
#include "user.hpp"
#include <vector>

class IDataManager
{
  public:
    virtual void save_user(User& user) = 0;
    virtual void load_users(std::vector<User>& users) const = 0;

    virtual void save_msg(Message& msg) = 0;
    virtual void load_msgs(std::vector<Message>& msgs) const = 0;
    virtual ~IDataManager(){};
};