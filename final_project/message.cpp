#include "message.hpp"

Message::Message(std::string sender, std::string receiver, std::string text, DateTime datetime)
    : sender(std::move(sender)), receiver(std::move(receiver)), text(std::move(text)), datetime(datetime)
{
}

bool Message::is_empty() const noexcept { return (this->sender == "") && (this->receiver == "") && (this->text == ""); }

std::istream& operator>>(std::istream& is, Message& msg)
{
    is >> msg.sender;
    is >> msg.receiver;
    // TODO: модифировать способ записи сообщения всем
    msg.receiver = msg.receiver == "-" ? "" : msg.receiver;
    is >> msg.datetime;

    std::getline(is, msg.text);
    if (msg.text.empty())
    {
        std::getline(is, msg.text);
    }
    msg.text = msg.text == "-" ? "" : msg.text;
    return is;
}
std::ostream& operator<<(std::ostream& os, const Message& msg)
{
    os << msg.sender;
    os << ' ';
    os << (msg.receiver == "" ? "-" : msg.receiver);
    os << ' ';
    os << msg.datetime;
    os << '\n';
    os << (msg.text == "" ? "-" : msg.text);
    return os;
}