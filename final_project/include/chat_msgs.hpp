#pragma once

namespace skillfactory
{
    enum class MsgType : unsigned char
    {
        SIGN_IN = 1,     // Вход в систему
        SIGN_UP = 2,     // Регистрация
        GET_USERS = 3,   // Получение списка пользователей
        GET_HISTORY = 4, // Получение истории сообщений
        SEND_MSG = 5     // Отправка сообщения
    };

    enum class MsgStatus : unsigned char
    {
        OK = 1,
        ERROR = 2
    };

    struct NetMessage
    {
        MsgType type;
        MsgStatus status;
        char data[1024 - sizeof(type) - sizeof(status)];
    };
} // namespace skillfactory