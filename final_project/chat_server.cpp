#include "chat_server.hpp"
#include "chat_msgs.hpp"
#include "file_data_manager.hpp"
#include "message.hpp"
#include "tcp_library.hpp"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
using namespace std;
namespace sf = skillfactory;

ChatServer::ChatServer(std::shared_ptr<IDataManager> data_manager) : data_manager(std::move(data_manager))
{
    // Создадим сокет
    if (!server.create_socket())
    {
        std::cout << "Socket creation failed.!" << '\n';
        exit(1);
    }

    // Привяжем сокет
    if (!server.bind_socket())
    {
        std::cout << "Socket binding failed.!" << '\n';
        exit(1);
    }

    // Поставим сервер на прием данных
    if (!server.enable_listening(5))
    {
        std::cout << "Socket is unable to listen for new connections.!" << '\n';
        exit(1);
    }

    // Загружаем пользователей и сообщения
    this->data_manager->load_users(this->users_table);
    this->data_manager->load_msgs(this->messages);

    cout << "Сервер успешно запущен!" << '\n';
}

bool ChatServer::sign_in_handle(int socket, const std::string& msg, std::string& client_login)
{
    // Выделяем из сообщения логин и хэш пароля
    std::cout << "Выделяем из сообщения логин и хэш пароля" << '\n';
    const auto* net_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    std::string login;
    std::string pass_hash;
    {
        std::stringstream sstr(net_msg->data);
        sstr >> login;
        sstr >> pass_hash;
    }
    std::cout << "Логин: " << login << ", пароль: " << pass_hash << '\n';
    std::cout << "Проверяем, что такой логин есть" << '\n';
    // Проверяем, что такой логин есть
    bool is_correct_user = this->users_table.find(login) != this->users_table.end();
    // Проверяем, что хэш пароля совпадает
    if (is_correct_user)
    {
        is_correct_user = is_correct_user && this->users_table[login] == pass_hash;
    }
    // Если все ок, то сохраняем логин и отправляем пользователю OK
    sf::NetMessage resp_msg;
    resp_msg.type = sf::MsgType::SIGN_IN;
    if (is_correct_user)
    {
        std::cout << "Пользователь авторизован, отправляем ответ" << '\n';
        resp_msg.status = sf::MsgStatus::OK;
    }
    else
    {
        std::cout << "Пользователь не найден" << '\n';
        // Иначе отправляем ошибку
        resp_msg.status = sf::MsgStatus::ERROR;
    }

    client_login = std::move(login);
    std::string response(reinterpret_cast<char*>(&resp_msg));
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }
    return is_correct_user;
}

bool ChatServer::sign_up_handle(int socket, const std::string& msg, std::string& client_login)
{
    // Считываем логин
    const auto* req_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    std::string login(req_msg->data);

    // Проверяем логин на занятость
    bool is_login_available = this->users_table.find(login) == this->users_table.end();

    // Если занят, отправляем error
    if (!is_login_available)
    {
        std::cout << "Логин занят" << '\n';
        sf::NetMessage resp_msg;
        resp_msg.type = sf::MsgType::SIGN_UP;
        resp_msg.status = sf::MsgStatus::ERROR;
        std::string response(reinterpret_cast<const char*>(&resp_msg));
        if (!TcpServer::send_msg(socket, response))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }
        return false;
    }
    else
    {
        // Если свободен, отправляем ОК
        std::cout << "Логин свободен, отправляем ответ" << '\n';
        sf::NetMessage resp_msg;
        resp_msg.type = sf::MsgType::SIGN_UP;
        resp_msg.status = sf::MsgStatus::OK;
        std::string response(reinterpret_cast<const char*>(&resp_msg));
        if (!TcpServer::send_msg(socket, response))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }
    }

    // Ждем сообщения с хэшем пароля
    std::string msg2;
    std::string pass_hash;
    {
        std::cout << "Ждем пароля" << '\n';
        if (!TcpServer::read_msg(socket, msg2))
        {
            std::cout << "Client Exited." << '\n';
            std::cout << "Server is Exiting..!" << '\n';
            return false;
        }
        if (msg2.empty())
        {
            std::cout << "Пришел пустой ответ вместо хэша пароля" << '\n';
            return false;
        }

        std::cout << "Пароль получен" << '\n';

        // TODO: проверять, что тип запроса такой же, а не другой
        const auto* req_msg2 = reinterpret_cast<const sf::NetMessage*>(msg2.data());
        std::stringstream sstr(req_msg2->data);
        sstr >> login;
        sstr >> pass_hash;
    }

    // TODO: синхронизировать доступ из разных потоков
    // Сохраняем пользователя в файл пользователей
    this->users_table.emplace(login, pass_hash);
    this->data_manager->save_user(login, pass_hash);
    client_login = std::move(login);

    // Авторизуем пользователя (привязываем сокет к этому пользователю)
    {
        std::cout << "Пользователь создан, отправляем ответ" << '\n';
        sf::NetMessage resp_msg;
        resp_msg.type = sf::MsgType::SIGN_UP;
        resp_msg.status = sf::MsgStatus::OK;
        std::string response(reinterpret_cast<const char*>(&resp_msg));
        if (!TcpServer::send_msg(socket, response))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }
    }
    return true;
}

bool ChatServer::get_users_handle(int socket)
{
    // Формируем сообщение
    sf::NetMessage resp_msg;
    resp_msg.type = sf::MsgType::GET_USERS;
    resp_msg.status = sf::MsgStatus::OK;
    std::string users_str;
    for (const auto& [login, pass] : this->users_table)
    {
        users_str += login + '\n';
    }
    strcpy(resp_msg.data, users_str.data());
    std::string response(reinterpret_cast<char*>(&resp_msg));
    std::cout << "Формируем список пользователе: " << '\n' << response;

    // Отправляем клиенту
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }

    std::cout << "Отправили список" << '\n';

    return true;
}

bool ChatServer::get_history_handle(int socket, const std::string& client_login)
{
    // Формируем сообщение
    std::stringstream sstr;

    // Показываем сообщения отправленные/адресованные текущему юзеру или адресованные всем
    for (const auto& msg : this->messages)
    {
        if (msg.sender == client_login || msg.receiver == client_login || msg.receiver == "")
        {
            sstr << msg << '\n';
        }
    }
    std::cout << "Формируем список сообщений: " << '\n' << sstr.str();

    sf::NetMessage resp_msg;
    resp_msg.type = sf::MsgType::GET_HISTORY;
    resp_msg.status = sf::MsgStatus::OK;
    strcpy(resp_msg.data, sstr.str().data());
    std::string response(reinterpret_cast<char*>(&resp_msg));

    // Отправляем клиенту
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }

    std::cout << "Отправили список" << '\n';

    return true;
}

bool ChatServer::send_msg_handle(int socket, const std::string& msg)
{
    // Распаковываем сообщение
    const auto* net_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    Message message;
    {
        std::stringstream ss_msg;
        ss_msg << net_msg->data;
        ss_msg >> message;
    }
    // Если пользователя не существует, то отправляем ошибку
    if (this->users_table.find(message.receiver) == this->users_table.end())
    {
        sf::NetMessage resp_msg;
        resp_msg.type = sf::MsgType::SEND_MSG;
        resp_msg.status = sf::MsgStatus::ERROR;
        std::string response(reinterpret_cast<char*>(&resp_msg));

        // Отправляем клиенту
        if (!TcpServer::send_msg(socket, response))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }
        return false;
    }

    // Отправляем статус ОК отправителю
    sf::NetMessage resp_msg;
    resp_msg.type = sf::MsgType::SEND_MSG;
    resp_msg.status = sf::MsgStatus::OK;
    std::string response(reinterpret_cast<char*>(&resp_msg));
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }

    // Отправляем сообщение получателю
    // TODO: нужен сокет получателя
    // Или при входе получателя отправлять ему сообщение

    // Сохраняем сообщение в файл
    this->data_manager->save_msg(message);

    // Сохраняем сообщение в массив
    this->messages.push_back(std::move(message));

    return true;
}

void ChatServer::client_handler(int socket)
{
    std::cout << "Client " << socket << " connected!" << '\n';

    std::string client_login;

    std::string msg;
    while (true)
    {
        // TODO: определять, что пользователь отключился
        msg.clear();
        if (!TcpServer::read_msg(socket, msg))
        {
            std::cout << "Client Exited." << '\n';
            std::cout << "Server is Exiting..!" << '\n';
            break;
        }
        if (msg.empty())
        {
            std::cout << "Похоже, что клиент отключился..." << '\n';
            return;
        }
        std::cout << "Data received from client (" << msg.size() << " bytes): " << msg << '\n';

        auto* net_msg = reinterpret_cast<sf::NetMessage*>(msg.data());
        std::cout << "Get command: " << static_cast<int>(net_msg->type) << '\n';

        switch (net_msg->type)
        {
            case sf::MsgType::SIGN_IN:
                {
                    std::cout << "SIGN_IN" << '\n';
                    this->sign_in_handle(socket, msg, client_login);
                    break;
                }
            case sf::MsgType::SIGN_UP:
                {
                    std::cout << "SIGN_UP" << '\n';
                    this->sign_up_handle(socket, msg, client_login);
                    break;
                }
            default:
                break;
        }

        if (client_login.empty())
        {
            continue;
        }

        switch (net_msg->type)
        {
            case sf::MsgType::SIGN_IN:
            case sf::MsgType::SIGN_UP:
                break;
            case sf::MsgType::GET_HISTORY:
                {
                    std::cout << "GET_HISTORY" << '\n';
                    this->get_history_handle(socket, client_login);
                    break;
                }
            case sf::MsgType::GET_USERS:
                {
                    std::cout << "GET_USERS" << '\n';
                    this->get_users_handle(socket);
                    break;
                }
            case sf::MsgType::SEND_MSG:
                {
                    std::cout << "SEND_MSG" << '\n';
                    this->send_msg_handle(socket, msg);
                    break;
                }
            default:
                {
                    std::cout << "No option" << '\n';
                    break;
                }
        }
    }
}

void ChatServer::accept_clients()
{
    while (true)
    {
        auto sock = this->server.wait_for_connection();
        if (sock)
        {
            std::thread client_thread([&]() { this->client_handler(*sock); });
            client_thread.detach();
        }
        // std::thread clientThread([this, clientSocket] { ClientHandler(clientSocket); });
        // clientThread.detach();
    }
}

void ChatServer::run()
{
    this->accept_clients();

    // auto sock = server.wait_for_connection();

    // if (!sock)
    // {
    //     std::cout << "Cannot establish connection" << '\n';
    //     return;
    // }

    // // Ожидаем подключения клиентов
    // for (size_t i = 0; i < 5; i++)
    // {
    //     auto sock = server.wait_for_connection();
    //     if (sock)
    //     {
    //         std::cout << "Get connection: " << *sock << "\n";
    //     }
    // }
}

int main(int argc, const char** argv)
{
    auto data_manager = std::make_shared<FileDataManager>("chat_data/users.txt", "chat_data/messages.txt");
    ChatServer server(data_manager);
    server.run();
    setlocale(LC_ALL, "");
    return 0;
}