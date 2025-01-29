#include "message.hpp"
#include <utility>

Message::Message() : sender_id(-1), receiver_id(-1), condition(0) {}

Message::Message(int sender_id, int receiver_id, std::string content, DateTime created_at, int condition)
    : sender_id(sender_id), receiver_id(receiver_id), content(std::move(content)), created_at(created_at)
{
}

bool Message::is_empty() const noexcept
{
    return (this->sender_id < 0) && (this->receiver_id < 0) && (this->content.size() == 0);
}

std::istream& operator>>(std::istream& is, Message& msg)
{
    is >> msg.sender_id;
    is >> msg.receiver_id;
    is >> msg.created_at;
    is >> msg.condition;
    std::getline(is, msg.content);
    if (msg.content.empty())
    {
        std::getline(is, msg.content);
    }
    msg.content = msg.content == "-" ? "" : msg.content;
    return is;
}
std::ostream& operator<<(std::ostream& os, const Message& msg)
{
    os << msg.sender_id;
    os << ' ';
    os << msg.receiver_id;
    os << ' ';
    os << msg.created_at;
    os << ' ';
    os << msg.condition;
    os << '\n';
    os << (msg.content == "" ? "-" : msg.content);
    return os;
}