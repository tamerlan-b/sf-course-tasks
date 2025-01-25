#pragma once

#include "datetime.hpp"
#include <istream>
#include <ostream>
#include <string>

class Message
{
  public:
    Message() = default;
    Message(std::string sender, std::string receiver, std::string text, DateTime datetime);
    virtual ~Message() = default;
    [[nodiscard]] bool is_empty() const noexcept;
    std::string sender;
    std::string receiver;
    std::string text;
    DateTime datetime;

    friend std::istream& operator>>(std::istream& is, Message& msg);
    friend std::ostream& operator<<(std::ostream& os, const Message& msg);
};