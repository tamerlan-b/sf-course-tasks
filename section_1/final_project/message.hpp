#pragma once

#include <string>
#include "user.hpp"
#include "datetime.hpp"
#include "3rd-party/nlohmann/json.hpp"

class Message
{
    public:
        Message(){}
        Message(std::string sender, std::string receiver, std::string text, DateTime datetime);
        ~Message(){}
        bool is_empty() const noexcept;
        std::string sender;
        std::string receiver;
        std::string text;
        DateTime datetime;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, sender, receiver, text, datetime)
};