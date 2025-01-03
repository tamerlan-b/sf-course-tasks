#pragma once

#include "message.hpp"
#include <sstream>
#include <string>
#include <vector>

namespace skillfactory
{
    enum class Commands : unsigned char
    {
        // Вход
        SIGN_IN = 1,       // нужны: логин + хэш пароля
                           // Регистрация
        SIGN_UP = 2,       // нужен сперва логин, затем пароль
                           // Получение истории сообщений
        GET_HISTORY = 3,   // нужен массив сообщений
                           // Получение списка пользователей
        GET_USER_LIST = 4, // массив логинов пользователей
                           // Отправка сообщения
        SEND_MSG = 5, // сообщение (логины отправителя и получателя + текст сообщения
                      // + время отправки)
        UNDEFINED = 255
    };

    enum class ResponseStatus : unsigned char
    {
        OK = 1,
        ERROR = 2,
        SERVER_ERROR = 3
    };

    struct Request
    {
        unsigned short command;
        std::string data;
    };

    struct Response
    {
        unsigned short command;
        std::string data;
    };

    template <Commands cmd> std::string pack(const std::vector<std::string>& users)
    {
        std::stringstream sstr;
        sstr << static_cast<unsigned char>(cmd) << '\n';
        for (const auto& user : users)
        {
            sstr << user << '\n';
        }
        return sstr.str();
    }

    template <Commands cmd> std::string pack(const std::vector<Message>& messages)
    {
        std::stringstream sstr;
        sstr << static_cast<unsigned char>(cmd) << '\n';
        for (const auto& msg : messages)
        {
            sstr << msg << '\n';
        }
        return sstr.str();
    }

    template <Commands cmd> std::string pack(const std::string& login)
    {
        std::stringstream sstr;
        sstr << static_cast<unsigned char>(cmd) << '\n';
        sstr << login << '\n';
        return sstr.str();
    }

    template <Commands cmd> std::string pack(const std::string& login, const std::string& pass_hash)
    {
        std::stringstream sstr;
        sstr << static_cast<unsigned char>(cmd) << '\n';
        sstr << login << ' ' << pass_hash << '\n';
        return sstr.str();
    }

    inline Commands unpack_type(const std::string& msg)
    {
        return msg.empty() ? Commands::UNDEFINED : static_cast<Commands>(msg[0]);
        // Commands cmd;
        // unsigned char cmd;
        // std::stringstream sstr;
        // sstr << msg;
        // sstr >> cmd;
        // return static_cast<Commands>(cmd);
    }

} // namespace skillfactory