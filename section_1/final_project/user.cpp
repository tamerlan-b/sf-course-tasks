#include "user.hpp"
#include "3rd-party/picosha2.h"         // хэширование SHA256

User::User(std::string login, std::string password) noexcept: login(login)
{
    // Вычисляем хэш пароля
    std::string pass_hash = picosha2::hash256_hex_string(password);
    this->pass_hash = pass_hash;
}

bool User::is_empty() const noexcept
{
    return (this->login == "") && (this->pass_hash == "");
}

bool User::check_login(std::string& login)
{
    return login == this->login;
}

bool User::check_password(std::string& password)
{
    std::string pass_hash = picosha2::hash256_hex_string(password);
    return pass_hash == this->pass_hash;
}

bool User::operator==(const User& other) const noexcept
{
    return (this->login == other.login) && (this->pass_hash == other.pass_hash);
}

const std::string& User::get_login() const noexcept
{
    return this->login;
}

std::ostream& operator<<(std::ostream& os, const User& user)
{
    os << user.login << " " << user.pass_hash;
    return os;
}