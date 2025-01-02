#include "app.hpp"
#include "3rd-party/picosha2.h"    // хэширование SHA256
#include "3rd-party/termcolor.hpp" // выделение цветом текста в терминале
#include "datetime.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <utility>

using namespace std;

enum Cmds
{
    SIGN_IN = 1,
    SIGN_UP = 2,
    EXIT = 3
};

enum ChatCmds
{
    SHOW_CHAT = 1,
    SHOW_USERS = 2,
    WRITE_MSG = 3,
    CHAT_EXIT = 4
};

App::App(std::shared_ptr<IDataManager> data_manager) : data_manager(std::move(data_manager))
{
    this->data_manager->load_users(this->users_table);
    this->data_manager->load_msgs(this->messages);
}

bool App::is_login_available(std::string& login) { return this->users_table.find(login) == this->users_table.end(); }

std::string App::create_user()
{
    // Считываем логин
    std::string login;
    bool repeat{false};
    do
    {
        cout << "Придумайте логин: ";
        cin >> login;
        if (repeat = !is_login_available(login); repeat)
        {
            cout << "Логин занят. Попробуйте другой" << "\n";
        }
    } while (repeat);

    // Считываем пароль
    std::string password;
    cout << "Придумайте пароль: ";
    cin >> password;

    // Добавляем пользователя в список
    std::string pass_hash = picosha2::hash256_hex_string(password);
    this->users_table.emplace(login, pass_hash);

    // Сохраняем пользователя в файл пользователей
    this->data_manager->save_user(login, pass_hash);

    cout << login << ", вы успешно зарегистрированы в Цидульке!" << "\n";
    return login;
}

std::string App::sign_in()
{
    std::string login;
    std::string password;
    cout << "Введите логин: ";
    cin >> login;
    cout << "Введите пароль: ";
    cin >> password;

    std::string in_pass_hash = picosha2::hash256_hex_string(password);
    if (this->users_table.find(login) != this->users_table.end() && this->users_table[login] == in_pass_hash)
    {
        cout << termcolor::green;
        cout << "Добро пожаловать, " << login << "\n";
        cout << termcolor::reset;
        return login; // TODO: убрать заглушку
    }
    else
    {
        cout << termcolor::red;
        cout << "Неверный логин или пароль" << "\n";
        cout << termcolor::reset;
        return "";
    }
}

std::string App::authorize()
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
                user_login = this->sign_in();
                done = !user_login.empty();
                break;
            case Cmds::SIGN_UP:
                cout << "Регистрация нового пользователя" << "\n";
                user_login = this->create_user();
                done = true;
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

void App::show_users(bool add_all) const noexcept
{
    auto show = [](const char* msg) { cout << " - " << msg << "\n"; };
    // Выводим всех пользователей (нужно в дебаге)
    for (const auto& [key, val] : this->users_table)
    {
        show(key.c_str());
    }
    // TODO: вернуть возможность писать всем пользователям
    // if(add_all)
    // {
    //     show(k, "All");
    // }
}

void show_message(const Message& msg, const std::string& user_name) noexcept
{
    cout << msg.datetime << '\n';
    msg.sender == user_name ? cout << termcolor::magenta : cout << termcolor::cyan;
    cout << msg.sender << ": " << termcolor::reset;
    cout << msg.text;
    if (msg.receiver != "")
    {
        cout << termcolor::yellow << " (to: " << msg.receiver << ")" << termcolor::reset;
    }
    cout << "\n";
}

void App::show_messages(const std::string& login) const noexcept
{
    for (const auto& msg : this->messages)
    {
        // Показываем сообщения отправленные/адресованные текущему юзеру или адресованные всем
        if (msg.sender == login || msg.receiver == login || msg.receiver == "")
        {
            show_message(msg, login);
        }
    }
}

void App::write_msg(const std::string& login)
{
    // TODO: вернуть поддержку сообщений с пробелом
    cout << "Введите логин получателя:" << "\n";
    this->show_users(true);

    std::string receiver_login;
    cin >> receiver_login;

    if (this->users_table.find(receiver_login) == this->users_table.end())
    {
        cout << "Введено неправильное значение" << "\n";
        return;
    }
    cout << "Введите сообщение: " << "\n";
    std::string text;
    cin.ignore();
    std::getline(cin, text);

    Message msg(login, receiver_login, text, DateTime(std::time(0)));
    this->messages.push_back(msg);

    // Сохраняем сообщение в файл
    this->data_manager->save_msg(msg);
    cout << "Сообщение отправлено" << "\n";
}

// TODO: переделать всё с user_id на логины пользователя!!!
void App::chat_menu(const std::string& login)
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

        std::string line_separator(10, '-');
        switch (choice)
        {
            case ChatCmds::SHOW_CHAT:
                cout << line_separator << '\n';
                cout << "Чат сообщений" << "\n";
                this->show_messages(login);
                cout << line_separator << '\n';
                done = false;
                break;
            case ChatCmds::SHOW_USERS:
                cout << line_separator << '\n';
                cout << "Пользователи чата" << "\n";
                this->show_users();
                cout << line_separator << '\n';
                done = false;
                break;
            case ChatCmds::WRITE_MSG:
                cout << "Написать сообщение" << "\n";
                this->write_msg(login);
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

void App::run()
{
    // Страница авторизации
    std::string user_login = this->authorize();
    if (!user_login.empty())
    {
        // Страница чата
        this->chat_menu(user_login);
    }
}