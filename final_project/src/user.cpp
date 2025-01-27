#include "user.hpp"

#include <utility>

User::User(std::string name, /*std::string surname, std::string email, */ std::string pass_hash)
    : name(std::move(name)), /*surname(std::move(surname)), email(std::move(email)), */ pass_hash(std::move(pass_hash))
{
}

[[nodiscard]] bool User::is_empty() const noexcept
{
    return (name.size() == 0) && /*(surname.size() == 0) && (email.size() == 0) &&*/ (pass_hash.size() == 0);
}

std::istream& operator>>(std::istream& is, User& user)
{
    is >> user.id;
    is >> user.name;
    // is >> user.surname;
    // is >> user.email;
    is >> user.pass_hash;
    return is;
}
std::ostream& operator<<(std::ostream& os, const User& user)
{
    os << user.id;
    os << ' ';
    os << user.name;
    os << ' ';
    // os << user.surname;
    // os << ' ';
    // os << user.email;
    // os << ' ';
    os << user.pass_hash;
    return os;
}