#pragma once

#include "datetime.hpp"
#include <istream>
#include <ostream>
#include <string>

class Message
{
  public:
    std::string sender;
    std::string receiver;
    std::string text;
    DateTime datetime;

    // int id;
    // int sender_id;
    // int reciever_id;
    // std::string content;
    // DateTime created_at;
    // int condition;

    Message() = default;
    Message(std::string sender, std::string receiver, std::string text, DateTime datetime);
    virtual ~Message() = default;
    [[nodiscard]] bool is_empty() const noexcept;

    friend std::istream& operator>>(std::istream& is, Message& msg);
    friend std::ostream& operator<<(std::ostream& os, const Message& msg);
};