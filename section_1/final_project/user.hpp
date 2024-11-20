#pragma once
#include <fstream>
#include <iostream>

class User
{
    private:
        std::string login;
        std::string pass_hash;   // Hash
    public:
        User() noexcept = default;
        User(std::string login, std::string password) noexcept;
        virtual ~User() = default;
        bool is_empty() const noexcept;
        bool check_login(std::string& login);
        bool check_password(std::string& password);
        bool operator==(const User& other) const noexcept;
        const std::string& get_login() const noexcept;
        friend std::fstream& operator >>(std::fstream& is, User& user);
	    friend std::ostream& operator <<(std::ostream& os, const User& user);
};