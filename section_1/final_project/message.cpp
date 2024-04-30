#include "message.hpp"


Message::Message(std::string sender, std::string receiver, std::string text, DateTime datetime)
: sender(sender), receiver(receiver), text(text), datetime(datetime)
{}

bool Message::is_empty() const noexcept
{
    return (this->sender == "") && (this->receiver == "") && (this->text == "");
}