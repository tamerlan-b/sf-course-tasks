#pragma once

#include "datetime.hpp"
#include <istream>
#include <ostream>
#include <string>

class Message
{
  public:
    int id;
    int sender_id;
    int receiver_id;
    std::string content;
    DateTime created_at;
    int condition;

    enum Condition
    {
        SENT = 0,
        RECEIVED = 1,
        READ = 2
    };

    Message();
    Message(int sender_id, int receiver_id, std::string content, DateTime created_at, int condition = 0);
    virtual ~Message() = default;
    [[nodiscard]] bool is_empty() const noexcept;

    friend std::istream& operator>>(std::istream& is, Message& msg);
    friend std::ostream& operator<<(std::ostream& os, const Message& msg);
};