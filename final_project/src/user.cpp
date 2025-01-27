#include "user.hpp"

#include <utility>

User::User(std::string login, std::string pass_hash) : login(std::move(login)), pass_hash(std::move(pass_hash)) {}

[[nodiscard]] bool User::is_empty() const noexcept { return (login.size() == 0) && (pass_hash.size() == 0); }

std::istream& operator>>(std::istream& is, User& user)
{
    is >> user.id;
    is >> user.login;
    is >> user.pass_hash;
    return is;
}
std::ostream& operator<<(std::ostream& os, const User& user)
{
    os << user.id;
    os << ' ';
    os << user.login;
    os << ' ';
    os << user.pass_hash;
    return os;
}