#include "chat_client.hpp"
#include "3rd-party/picosha2.h" // хэширование SHA256
// #include "3rd-party/termcolor.hpp" // выделение цветом текста в терминале
// #include "chat_msgs.hpp"
#include "chat_msgs.hpp"
#include "message.hpp"
#include <chrono>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
namespace sf = skillfactory;
using namespace std;

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

bool ChatClient::sign_up(std::string& login)
{
    // Считываем логин
    if (!login.empty())
    {
        login.clear();
    }
    do
    {
        cout << "Придумайте логин: ";
        cin >> login;

        // Отправляем запрос с логином на сервер
        sf::NetMessage req_msg;
        req_msg.type = sf::MsgType::SIGN_UP;
        req_msg.status = sf::MsgStatus::OK;
        strcpy(req_msg.data, login.data());
        std::string req(reinterpret_cast<const char*>(&req_msg));
        if (!client.send_msg(req))
        {
            std::cout << "Сервер недоступен" << '\n';
            continue;
        }

        // Ждем ответ
        std::string response;
        if (!this->wait_for_response(response, sf::MsgType::SIGN_UP))
        {
            std::cout << "Проблемы с проверкой доступности логина" << '\n';
            continue;
        }

        auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());

        if (resp_msg->status == sf::MsgStatus::OK)
        {
            break;
        }
        cout << "Логин занят. Попробуйте другой" << "\n";

    } while (true);

    // Считываем пароль
    std::string password;
    cout << "Придумайте пароль: ";
    cin >> password;

    // Отправляем запрос с логином и хэшем пароля на сервер
    std::string pass_hash = picosha2::hash256_hex_string(password);

    sf::NetMessage req_msg;
    req_msg.type = sf::MsgType::SIGN_UP;
    req_msg.status = sf::MsgStatus::OK;
    strcpy(req_msg.data, (login + "\n" + pass_hash).data());
    std::string req(reinterpret_cast<const char*>(&req_msg));
    // TODO: не отправлять повторно логин
    if (!client.send_msg(req))
    {
        std::cout << "Сервер недоступен" << '\n';
        return false;
    }

    // Ждем ответ
    std::string response;
    if (!this->wait_for_response(response, sf::MsgType::SIGN_UP))
    {
        std::cout << "Проблемы с проверкой доступности логина" << '\n';
        return false;
    }

    auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());
    bool res = resp_msg->status == sf::MsgStatus::OK;

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
    sf::NetMessage req_msg;
    req_msg.type = sf::MsgType::SIGN_IN;
    req_msg.status = sf::MsgStatus::OK;
    strcpy(req_msg.data, (login + "\n" + pass_hash).data());
    std::string request(reinterpret_cast<char*>(&req_msg));
    if (!client.send_msg(request))
    {
        std::cout << "Сервер недоступен" << '\n';
        return false;
    }

    // Ждем ответ
    std::string response;
    if (!this->wait_for_response(response, sf::MsgType::SIGN_IN))
    {
        std::cout << "Проблемы с проверкой доступности логина" << '\n';
        return false;
    }
    auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());
    bool has_verified = resp_msg->status == sf::MsgStatus::OK;
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

bool ChatClient::wait_for_response(std::string& response, sf::MsgType msg_type, const int timeout)
{
    const int sleep_time_ms{10};
    int total_sleep_time_ms{0};
    while (total_sleep_time_ms < timeout)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
        total_sleep_time_ms += sleep_time_ms;
        // TODO: оценить правильность поиска
        for (auto it = this->server_msgs.begin(); it != this->server_msgs.end(); ++it)
        {
            const auto* msg_net = reinterpret_cast<const sf::NetMessage*>(it->data());
            // Если нашли нужное сообщение
            if (msg_net->type == msg_type)
            {
                // Сохраняем найденное сообщение
                response = *it;
                // Удаляем его из буфера
                this->server_msgs.erase(it);
                // Выходим из метода
                return true;
            }
        }
    }

    return false;
}

void deserialize(const std::string& msg, std::vector<ChatClient::SimpleUser>& users)
{
    {
        std::stringstream sstr;
        sstr << msg;

        // сначала считываем количество пользователей
        size_t num_users;
        sstr >> num_users;
        users.reserve(num_users);
        while (!sstr.eof())
        {
            int id;
            std::string login;
            sstr >> id;
            sstr >> login;
            if (!login.empty())
            {
                users.push_back({id, std::move(login)});
            }
        }
    }
}

void deserialize(const std::string& msg, std::vector<Message>& messages)
{
    std::stringstream sstr;
    sstr << msg;
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

void print(const std::vector<ChatClient::SimpleUser>& users)
{
    for (const auto& user : users)
    {
        std::cout << user.id << ": " << user.login << '\n';
    }
}

int ChatClient::get_user_by_id(int id) const
{
    auto it = std::find_if(this->users.begin(), this->users.end(), [id](const SimpleUser& u) { return u.id == id; });
    if (it != this->users.end())
    {
        return std::distance(this->users.begin(), it);
    }
    return -1;
}

bool ChatClient::get_users()
{
    // Формируем запрос и отправляем запрос на сервер
    {
        sf::NetMessage req_msg;
        req_msg.type = sf::MsgType::GET_USERS;
        req_msg.status = sf::MsgStatus::OK;
        std::string request(reinterpret_cast<char*>(&req_msg));
        if (!client.send_msg(request))
        {
            std::cout << "Сервер недоступен" << '\n';
            return false;
        }
    }

    // Ожидаем получения нужного ответа
    std::string response;
    if (!this->wait_for_response(response, sf::MsgType::GET_USERS))
    {
        std::cout << "Статус от сервера не позволяет считать данные" << '\n';
        return false;
    }

    // Распаковываем ответ и сохраняем список пользователей
    auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());

    // Сохраняем пользователей в поле класса
    this->users.clear();
    deserialize(resp_msg->data, this->users);

    // Отображаем его
    // std::cout << "Список пользователей:" << '\n';
    for (const auto& user : this->users)
    {
        std::cout << user.id << ": " << user.login << '\n';
    }
    return true;
}

void show_message(const Message& msg) noexcept
{
    cout << msg.created_at << '\n';
    // msg.sender == user_name ? cout << termcolor::magenta : cout << termcolor::cyan;
    cout << msg.sender_id << ": "; // << termcolor::reset;
    cout << msg.content;
    if (msg.receiver_id >= 0)
    {
        // cout << termcolor::yellow << " (to: " << msg.receiver << ")" << termcolor::reset;
        cout << " (to: " << msg.receiver_id << ")";
    }
    cout << "\n";
}

bool ChatClient::get_history()
{
    // Формируем запрос и отправляем запрос на сервер
    {
        sf::NetMessage req_msg;
        req_msg.type = sf::MsgType::GET_HISTORY;
        req_msg.status = sf::MsgStatus::OK;
        std::string request(reinterpret_cast<char*>(&req_msg));
        if (!client.send_msg(request))
        {
            std::cout << "Сервер недоступен" << '\n';
            return false;
        }
    }

    // Ждем ответ
    std::string response;
    if (!this->wait_for_response(response, sf::MsgType::GET_HISTORY))
    {
        std::cout << "Проблемы с получением ответа от сервера" << '\n';
        return false;
    }
    auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());
    if (resp_msg->status != sf::MsgStatus::OK)
    {
        std::cout << "Статус от сервера не позволяет считать данные" << '\n';
        return false;
    }

    // TODO: сохранять в переменную класса
    // Распаковываем ответ и сохраняем список сообщений
    std::vector<Message> messages;
    deserialize(resp_msg->data, messages);

    // Отображаем его
    // std::cout << "Чат:" << '\n';
    for (const auto& one_msg : messages)
    {
        show_message(one_msg);
    }
    return true;
}

bool ChatClient::send_msg(const std::string& login)
{
    // Узнаем у пользователя необходимые данные: кому отправить и что написать

    // TODO: при отсутствии загруженного списка пользователей неявно просить его у сервера
    if (this->users.size() == 0)
    {
        std::cout << "Не загружен список пользователей - загрузите его отдельно" << '\n';
        return false;
    }
    std::cout << "Введите id получателя:" << '\n';
    print(this->users);
    int user_id;
    std::cin >> user_id;

    // Проверяем существование адресата
    int user_index = this->get_user_by_id(user_id);
    if (user_index < 0)
    {
        std::cout << "Нет пользователя с таким id" << '\n';
        return false;
    }

    cout << "Введите сообщение: " << "\n";
    std::string text;
    cin.ignore();
    std::getline(cin, text);

    Message msg(-1, user_id, text, DateTime(std::time(0)));

    // Формируем запрос и отправляем на сервер
    sf::NetMessage req_msg;
    req_msg.type = sf::MsgType::SEND_MSG;
    req_msg.status = sf::MsgStatus::OK;
    {
        std::stringstream ss_msg;
        ss_msg << msg;
        strcpy(req_msg.data, ss_msg.str().data());
    }
    std::string request(reinterpret_cast<char*>(&req_msg));
    if (!client.send_msg(request))
    {
        std::cout << "Сервер недоступен" << '\n';
        return false;
    }

    // Ждем ответ
    std::string response;
    if (!this->wait_for_response(response, sf::MsgType::SEND_MSG))
    {
        std::cout << "Проблемы с получением ответа от сервера" << '\n';
        return false;
    }
    auto* resp_msg = reinterpret_cast<sf::NetMessage*>(response.data());
    if (resp_msg->status != sf::MsgStatus::OK)
    {
        std::cout << "Статус от сервера не позволяет считать данные" << '\n';
        return false;
    }
    // Отображаем его
    return true;
}

void ChatClient::render_chat_menu()
{
    if (this->server_msgs.empty())
    {
        cout << ChatCmds::SHOW_CHAT << ". Посмотреть чат" << "\n";
    }
    else
    {
        cout << ChatCmds::SHOW_CHAT << ". Посмотреть чат (+" << this->server_msgs.size() << ")" << "\n";
    }
    cout << ChatCmds::SHOW_USERS << ". Посмотреть список пользователей" << "\n";
    cout << ChatCmds::WRITE_MSG << ". Написать сообщение" << "\n";
    cout << ChatCmds::CHAT_EXIT << ". Выход" << "\n";
}

void ChatClient::remove_new_messages()
{
    for (auto it = this->server_msgs.begin(); it != this->server_msgs.end();)
    {
        const auto* net_msg = reinterpret_cast<const sf::NetMessage*>(it->data());
        if (net_msg->type == sf::MsgType::SEND_MSG)
        {
            it = this->server_msgs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ChatClient::chat_menu(const std::string& login)
{
    bool done{false};
    while (!done)
    {
        this->render_chat_menu();
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
                this->get_history();
                this->remove_new_messages();
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

void ChatClient::listen_server()
{
    while (true)
    {
        std::string response;
        if (!client.read_msg(response))
        {
            std::cout << "Проблемы с получением ответа от сервера" << '\n';
        }
        // std::cout << "Получено сообщение от сервера" << "\n";
        this->server_msgs.emplace_front(std::move(response));
        if (reinterpret_cast<sf::NetMessage*>(this->server_msgs.front().data())->type == sf::MsgType::SEND_MSG)
        {
            this->render_chat_menu();
        }
    }
}

void ChatClient::run()
{
    // Запускаем параллельное прослушивание сообщений от сервера
    std::thread listener_thread([&]() { this->listen_server(); });
    // Страница авторизации
    std::string user_login = this->authorize();
    if (!user_login.empty())
    {
        // Страница чата
        this->chat_menu(user_login);
    }
    listener_thread.detach();
}

int main(int argc, const char** argv)
{
    setlocale(LC_ALL, "");
    ChatClient client;
    client.run();
    return 0;
}