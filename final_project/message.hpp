#pragma once

#include <string>
#include "datetime.hpp"
#include <fstream>

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

        friend std::fstream& operator >>(std::fstream& is, Message& msg);
	    friend std::ostream& operator <<(std::ostream& os, const Message& msg);
};