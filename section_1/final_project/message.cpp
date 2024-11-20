#include "message.hpp"


Message::Message(std::string sender, std::string receiver, std::string text, DateTime datetime)
: sender(sender), receiver(receiver), text(text), datetime(datetime)
{}

bool Message::is_empty() const noexcept
{
    return (this->sender == "") && (this->receiver == "") && (this->text == "");
}

std::fstream& operator >>(std::fstream& is, Message& msg)
{
    is >> msg.sender;
    is >> msg.receiver;
    // TODO: модифировать способ записи сообщения всем
    msg.receiver = msg.receiver == "-" ? "" : msg.receiver;
    is >> msg.text;
    msg.text = msg.text == "-" ? "" : msg.text;
    is >> msg.datetime;
    return is;
}
std::ostream& operator <<(std::ostream& os, const Message& msg)
{
    os << msg.sender;
    os << ' ';
    os << (msg.receiver == "" ? "-" : msg.receiver);
    os << ' ';
    os << (msg.text == "" ? "-" : msg.text);
    os << ' ';
    os << msg.datetime;
    return os;
}