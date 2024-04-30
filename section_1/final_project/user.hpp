#pragma once
#include <iostream>
#include "3rd-party/nlohmann/json.hpp"

class User
{
    private:
        std::string login;
        std::string pass_hash;   // Hash
    public:
        User() noexcept{}
        User(std::string login, std::string password) noexcept;
        virtual ~User(){}
        bool is_empty() const noexcept;
        bool check_login(std::string& login);
        bool check_password(std::string& password);
        bool operator==(const User& other) const noexcept;
        const std::string& get_login() const noexcept;
        friend std::ostream& operator<<(std::ostream&, const User& user);

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, login, pass_hash)
};