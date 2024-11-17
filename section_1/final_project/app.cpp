#include "app.hpp"
#include <iostream>
#include <stdlib.h> 
#include <fstream>
#include <limits>
#include <ctime>
#include <algorithm>
#include "3rd-party/termcolor.hpp"    // выделение цветом текста в терминале
#include "3rd-party/nlohmann/json.hpp"    // сериализация/десериализация json-формата
#include "datetime.hpp"

using namespace std;
using json = nlohmann::json;

enum Cmds{
    SIGN_IN=1,
    SIGN_UP=2,
    EXIT=3
};

enum ChatCmds{
    SHOW_CHAT = 1, 
    SHOW_USERS = 2, 
    WRITE_MSG = 3, 
    CHAT_EXIT = 4
};

void App::load_users()
{
    std::ifstream users_file(this->users_path);
    if(users_file.peek() != std::ifstream::traits_type::eof())   // Если файл непустой
    {
        json j = json::parse(users_file);
        users_file.close();
        for (auto& [key, value] : j.items())
        {
            this->users.push_back(value);
        }
    }
    else
    {
        users_file.close();
    }
}

void App::save_user(const User& user) const
{
    json j(this->users);
    std::ofstream f(this->users_path, std::ios_base::trunc);
    f << j.dump(4);
    f.close();
}

void App::load_messages()
{
    std::ifstream msgs_file(this->messages_path);
    if(msgs_file.peek() != std::ifstream::traits_type::eof())   // Если файл непустой
    {
        json j = json::parse(msgs_file);
        msgs_file.close();
        for (auto& [key, value] : j.items())
        {
            this->messages.push_back(value);
        }
    }
    else
    {
        msgs_file.close();
    }
}

void App::save_message(const Message& msg) const
{
    json j(this->messages);
    std::ofstream f(this->messages_path, std::ios_base::trunc);
    f << j.dump(4);
    f.close();
}

App::App(std::string users_path, std::string messages_path)
: users_path(users_path), messages_path(messages_path)
{
    
    // Считываем пользователей из файла
    this->load_users();

    // Считываем историю сообщений
    this->load_messages();
    
}

bool App::is_login_available(std::string& login)
{
    auto predicate = [&login](User &u){ return u.check_login(login);};
    return std::none_of(this->users.begin(), this->users.end(), predicate);
}

int App::create_user()
{
    // Считываем логин
    std::string login;
    bool repeat{false};
    do
    {
        cout << "Придумайте логин: ";
        cin >> login;
        if(repeat = !is_login_available(login))
            cout << "Логин занят. Попробуйте другой" << "\n";
    }
    while(repeat);
    
    // Считываем пароль
    std::string password;
    cout << "Придумайте пароль: ";
    cin >> password;

    // Добавляем пользователя в список
    User new_user(login, password);
    this->users.push_back(new_user);

    // Сохраняем пользователя в файл пользователей
    this->save_user(new_user);

    cout << login << ", вы успешно зарегистрированы в Цидульке!" << "\n";
    return this->users.size() - 1;
}

int App::sign_in()
{
    std::string login, password;
    cout << "Введите логин: ";
    cin >> login;
    cout << "Введите пароль: ";
    cin >> password;
    
    std::vector<User>::iterator it = std::find(this->users.begin(), this->users.end(), User(login, password));
    if (it != this->users.end())
    {
        cout << termcolor::green;
        cout << "Добро пожаловать, " << it->get_login() << "\n";
        cout << termcolor::reset;
        return static_cast<int>(std::distance(this->users.begin(), it));
    }
    else
    {
        cout << termcolor::red;
        cout << "Неверный логин или пароль" << "\n";
        cout << termcolor::reset;
        return -1;
    }
}

int App::authorize()
{
    int user_id{-1};
    bool done{false};
    while(!done)
    {
        cout << "Добро пожаловать в чат Цидулька" << "\n";
        cout << "\t" << Cmds::SIGN_IN << ". Войти" << "\n";
        cout << "\t" << Cmds::SIGN_UP << ". Зарегистрироваться" << "\n";
        cout << "\t" << Cmds::EXIT << ". Выход" << "\n";
        int choice;
        cin >> choice;

        if(cin.fail())
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
                user_id = this->sign_in();
                done = !(user_id < 0);
                break;
            case Cmds::SIGN_UP:
                cout << "Регистрация нового пользователя" << "\n";
                user_id = this->create_user();
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
    return user_id;
}

void App::show_users(const bool add_all) const noexcept
{
    auto show = [](int i, const char* msg){ cout << i << ". " << msg << "\n"; };
    // Выводим всех пользователей (нужно в дебаге)
    for (size_t i = 0; i < this->users.size(); i++)
    {
        show(i, this->users[i].get_login().c_str());
    }
    if(add_all)
    {
        show(this->users.size(), "All");
    }
}

void show_message(const Message& msg, const std::string& user_name) noexcept
{
    cout << msg.datetime << '\n';
    msg.sender == user_name ? cout << termcolor::magenta : cout << termcolor::cyan;
    cout << msg.sender << ": " << termcolor::reset;
    cout << msg.text;
    if(msg.receiver != "")
        cout << termcolor::yellow << " (to: " << msg.receiver << ")" << termcolor::reset;
    cout << "\n";
}

void App::show_messages(const int user_id) const noexcept
{
    const std::string& user_name = this->users[user_id].get_login();
    for (size_t i = 0; i < this->messages.size(); i++)
    {   
        // Показываем сообщения отправленные/адресованные текущему юзеру или адресованные всем
        const auto& msg = this->messages[i];
        if(msg.sender == user_name || msg.receiver == user_name || msg.receiver == "")
        {
            show_message(msg, user_name);
        }
    }
    
}

void App::write_msg(const int user_id)
{
    cout << "Выберите получателя сообщения:" << "\n";
    this->show_users(true);
    int receiver_id{-1};
    cin >> receiver_id;
    if(receiver_id < 0 || receiver_id > this->users.size())
    {
        cout << "Введено неправильное значение" << "\n";
        return;
    }
    cout << "Введите сообщение: " << "\n";
    std::string text;
    cin.ignore();
    std::getline(cin, text);

    std::string receiver{receiver_id < this->users.size() ? this->users[receiver_id].get_login(): ""};

    Message msg(this->users[user_id].get_login(), receiver, text, DateTime(std::time(0)));
    this->messages.push_back(msg);

    // Сохраняем сообщение в файл
    this->save_message(msg);
    cout << "Сообщение отправлено" << "\n";
}

void App::chat_menu(const int user_id)
{
    bool done{false};
    while(!done)
    {
        cout << ChatCmds::SHOW_CHAT << ". Посмотреть чат" << "\n";
        cout << ChatCmds::SHOW_USERS << ". Посмотреть список пользователей" << "\n";
        cout << ChatCmds::WRITE_MSG << ". Написать сообщение" << "\n";
        cout << ChatCmds::CHAT_EXIT << ". Выход" << "\n";
        int choice;
        cin >> choice;

        if(cin.fail())
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
                this->show_messages(user_id);
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
                this->write_msg(user_id);
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
    int user_id = this->authorize();
    if(user_id >= 0)
    {
        // Страница чата
        this->chat_menu(user_id);
    }
    
}