#pragma once
#include <istream>
#include <ostream>
#include <string>

class User
{
  public:
    int id;
    std::string login;
    std::string pass_hash;

    User() = default;
    virtual ~User() = default;

    User(std::string login, std::string pass_hash);
    [[nodiscard]] bool is_empty() const noexcept;
    friend std::istream& operator>>(std::istream& is, User& user);
    friend std::ostream& operator<<(std::ostream& os, const User& user);
};