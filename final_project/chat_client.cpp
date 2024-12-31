#include "chat_client.hpp"
#include "3rd-party/picosha2.h" // хэширование SHA256
// #include "3rd-party/termcolor.hpp" // выделение цветом текста в терминале
#include "chat_msgs.hpp"
#include "message.hpp"
#include <sstream>
#include <string>
#include <vector>
namespace sf = skillfactory;
#include <iostream>
#include <limits>
using namespace std;

inline sf::ResponseStatus unpack_response_type(const std::string& msg)
{
    return static_cast<sf::ResponseStatus>(msg[0]);
}

ChatClient::ChatClient()
{
    if (!client.create_socket())
    {
        std::cout << "Не удалось создать сокет!" << '\n';
        exit(1);
    }

    if (!client.connect_server())
    {
        std::cout << "Не удалось установить соединение с сервером!" << '\n';
        exit(1);
    }
}
ChatClient::~ChatClient() {}

bool ChatClient::sign_up(std::string& login)
{
    // Считываем логин
    if (!login.empty())
    {
        login.clear();
    }
    bool is_login_available{false};
    do
    {
        cout << "Придумайте логин: ";
        cin >> login;

        // Отправляем запрос с логином на сервер
        std::string request = sf::pack<sf::Commands::SIGN_UP>(login);
        if (!client.send_msg(request))
        {
            std::cout << "Сервер недоступен" << '\n';
            continue;
        }

        // Ждем ответ
        std::string response;
        if (!client.read_msg(response))
        {
            std::cout << "Проблемы с проверкой доступности логина" << '\n';
            continue;
        }

        is_login_available = unpack_response_type(response) == sf::ResponseStatus::OK;

        if (!is_login_available)
        {
            cout << "Логин занят. Попробуйте другой" << "\n";
        }
    } while (!is_login_available);

    // Считываем пароль
    std::string password;
    cout << "Придумайте пароль: ";
    cin >> password;

    // Отправляем запрос с логином и хэшем пароля на сервер
    std::string pass_hash = picosha2::hash256_hex_string(password);
    // TODO: не отправлять повторно логин
    std::string request = sf::pack<sf::Commands::SIGN_UP>(pass_hash);
    if (!client.send_msg(request))
    {
        std::cout << "Сервер недоступен" << '\n';
        return false;
    }

    // Ждем ответ
    std::string response;
    if (!client.read_msg(response))
    {
        std::cout << "Проблемы с проверкой доступности логина" << '\n';
        return false;
    }

    bool res = unpack_response_type(response) == sf::ResponseStatus::OK;

    cout << login << ", вы успешно зарегистрированы в Цидульке!" << "\n";
    return res;
}

bool ChatClient::sign_in(std::string& login)
{
    if (!login.empty())
    {
        login.clear();
    }
    std::string password;
    cout << "Введите логин: ";
    cin >> login;
    cout << "Введите пароль: ";
    cin >> password;

    std::string pass_hash = picosha2::hash256_hex_string(password);

    // Отправляем запрос на сервер
    std::string request = sf::pack<sf::Commands::SIGN_IN>(login, pass_hash);
    std::cout << "Отправляем запрос на сервер: " << request << '\n';
    if (!client.send_msg(request))
    {
        std::cout << "Сервер недоступен" << '\n';
        return false;
    }

    // Ждем ответ
    std::cout << "Ждем ответ от сервера" << '\n';
    std::string response;
    if (!client.read_msg(response))
    {
        std::cout << "Проблемы с проверкой доступности логина" << '\n';
        return false;
    }
    bool has_verified = unpack_response_type(response) == sf::ResponseStatus::OK;

    if (has_verified)
    {
        // cout << termcolor::green;
        cout << "Добро пожаловать, " << login << "\n";
        // cout << termcolor::reset;
    }
    else
    {
        // cout << termcolor::red;
        cout << "Неверный логин или пароль" << "\n";
        // cout << termcolor::reset;
    }
    return has_verified;
}

std::string ChatClient::authorize()
{
    std::string user_login;
    bool done{false};
    while (!done)
    {
        cout << "Добро пожаловать в чат Цидулька" << "\n";
        cout << "\t" << Cmds::SIGN_IN << ". Войти" << "\n";
        cout << "\t" << Cmds::SIGN_UP << ". Зарегистрироваться" << "\n";
        cout << "\t" << Cmds::EXIT << ". Выход" << "\n";
        int choice;
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка ввода. Попробуйте снова" << "\n";
            continue;
        }

        switch (choice)
        {
            case Cmds::SIGN_IN:
                cout << "Вход в Цидульку" << "\n";
                done = this->sign_in(user_login);
                break;
            case Cmds::SIGN_UP:
                cout << "Регистрация нового пользователя" << "\n";
                done = this->sign_up(user_login);
                break;
            case Cmds::EXIT:
                cout << "До скорой встречи!" << "\n";
                done = true;
                break;
            default:
                cout << "Такой команды нет!" << "\n";
                break;
        }
    }
    return user_login;
}

bool ChatClient::get_users()
{
    // Формируем запрос и отправляем запрос на сервер
    {
        std::string request;
        request += static_cast<unsigned char>(sf::Commands::GET_USER_LIST);
        if (!client.send_msg(request))
        {
            std::cout << "Сервер недоступен" << '\n';
            return false;
        }
    }

    // Ждем ответ
    std::string response;
    if (!client.read_msg(response))
    {
        std::cout << "Проблемы с получением ответа от сервера" << '\n';
        return false;
    }

    if (unpack_response_type(response) != sf::ResponseStatus::OK)
    {
        std::cout << "Статус от сервера не позволяет считать данные" << '\n';
        return false;
    }

    // Распаковываем ответ и сохраняем список пользователей
    std::vector<std::string> users;
    {
        std::stringstream sstr;
        sstr << response;
        unsigned char cmd;
        sstr >> cmd;
        while (!sstr.eof())
        {
            std::string login;
            sstr >> login;
            if (!login.empty())
            {
                users.push_back(std::move(login));
            }
        }
    }

    // Отображаем его
    std::cout << "Список пользователей:" << '\n';
    for (const auto& user : users)
    {
        std::cout << " - " << user << '\n';
    }
    return true;
}

void show_message(const Message& msg) noexcept
{
    cout << msg.datetime << '\n';
    // msg.sender == user_name ? cout << termcolor::magenta : cout << termcolor::cyan;
    cout << msg.sender << ": "; // << termcolor::reset;
    cout << msg.text;
    if (msg.receiver != "")
    {
        // cout << termcolor::yellow << " (to: " << msg.receiver << ")" << termcolor::reset;
        cout << " (to: " << msg.receiver << ")";
    }
    cout << "\n";
}

bool ChatClient::get_history()
{
    // Формируем запрос и отправляем запрос на сервер
    {
        std::string request;
        request += static_cast<unsigned char>(sf::Commands::GET_HISTORY);
        if (!client.send_msg(request))
        {
            std::cout << "Сервер недоступен" << '\n';
            return false;
        }
    }

    // Ждем ответ
    std::string response;
    if (!client.read_msg(response))
    {
        std::cout << "Проблемы с получением ответа от сервера" << '\n';
        return false;
    }

    if (unpack_response_type(response) != sf::ResponseStatus::OK)
    {
        std::cout << "Статус от сервера не позволяет считать данные" << '\n';
        return false;
    }

    // TODO: сохранять в переменную класса
    // Распаковываем ответ и сохраняем список сообщений
    std::vector<Message> messages;
    {
        std::stringstream sstr;
        sstr << response;
        unsigned char cmd;
        sstr >> cmd;
        while (!sstr.eof())
        {
            Message one_msg;
            sstr >> one_msg;
            if (!one_msg.is_empty())
            {
                messages.push_back(std::move(one_msg));
            }
        }
    }

    // Отображаем его
    std::cout << "Чат:" << '\n';
    for (const auto& one_msg : messages)
    {
        show_message(one_msg);
    }
    return true;
}

bool ChatClient::send_msg(const std::string& login)
{
    // Узнаем у пользователя необходимые данные: кому отправить и что написать
    // TODO: вернуть поддержку сообщений с пробелом

    // Формируем запрос и отправляем на сервер

    // Ждем ответ
    // Отображаем его
    return true;
}

void ChatClient::chat_menu(const std::string& login)
{
    bool done{false};
    while (!done)
    {
        cout << ChatCmds::SHOW_CHAT << ". Посмотреть чат" << "\n";
        cout << ChatCmds::SHOW_USERS << ". Посмотреть список пользователей" << "\n";
        cout << ChatCmds::WRITE_MSG << ". Написать сообщение" << "\n";
        cout << ChatCmds::CHAT_EXIT << ". Выход" << "\n";
        int choice;
        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка ввода. Попробуйте снова" << "\n";
            continue;
        }

        // TODO: в параллельном потоке слушать сервер на предмет получения новых сообщений

        std::string line_separator(10, '-');
        switch (choice)
        {
            case ChatCmds::SHOW_CHAT:
                cout << line_separator << '\n';
                cout << "Чат сообщений" << "\n";
                this->get_history();
                cout << line_separator << '\n';
                done = false;
                break;
            case ChatCmds::SHOW_USERS:
                cout << line_separator << '\n';
                cout << "Пользователи чата" << "\n";
                this->get_users();
                cout << line_separator << '\n';
                done = false;
                break;
            case ChatCmds::WRITE_MSG:
                cout << "Написать сообщение" << "\n";
                this->send_msg(login);
                done = false;
                break;
            case ChatCmds::CHAT_EXIT:
                cout << "До скорой встречи!" << "\n";
                done = true;
                break;
            default:
                cout << "Такой команды нет!" << "\n";
                break;
        }
    }
}

void ChatClient::run()
{
    // Страница авторизации
    std::string user_login = this->authorize();
    if (!user_login.empty())
    {
        // Страница чата
        this->chat_menu(user_login);
    }
}

int main(int argc, const char** argv)
{
    setlocale(LC_ALL, "");
    ChatClient client;
    client.run();
    return 0;
}