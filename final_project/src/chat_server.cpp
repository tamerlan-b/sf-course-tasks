#include "chat_server.hpp"
#include "chat_msgs.hpp"
#include "file_data_manager.hpp"
#include "logger.hpp"
#include "logger_interface.hpp"
#include "message.hpp"
#include "tcp_library.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
using namespace std;
namespace sf = skillfactory;

#if defined(_WIN32)
#define PLATFORM_NAME "windows 32-bit" // Windows 32-bit
#elif defined(_WIN64)
#define PLATFORM_NAME "windows 64-bit" // Windows 64-bit
#elif defined(__ANDROID__)
#define PLATFORM_NAME "android" // Android
#elif defined(__linux__)
#define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos и другие
#elif TARGET_OS_IPHONE == 1
#define PLATFORM_NAME "ios" // Apple iOS
#elif TARGET_OS_MAC == 1
#define PLATFORM_NAME "osx" // Apple OSX
#else
#define PLATFORM_NAME "OS not detected" // ОС не распознана
#endif

#if defined(__linux__)
#include <sys/utsname.h> // Для uname()
#endif

ChatServer::ChatServer(std::unique_ptr<skillfactory::ILogger>& logger, std::unique_ptr<IDataManager>& data_manager)
    : data_manager(std::move(data_manager)), logger(std::move(logger))
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
    const auto* net_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    std::string login;
    std::string pass_hash;
    {
        std::stringstream sstr(net_msg->data);
        sstr >> login;
        sstr >> pass_hash;
    }
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
        std::cout << "Пользователь " << login << " авторизован, отправляем ответ" << '\n';
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
        std::cout << "Пользователь " << client_login << " создан, отправляем ответ" << '\n';
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

    // Отправляем клиенту
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }

    std::cout << "Отправили список пользователей" << '\n';

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

    // TODO: разбивать сообщения на куски при превышении размера

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

    std::cout << "Отправили список сообщений пользователю " << client_login << '\n';

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
        std::cout << "Выбранного пользователя не существует (" << '\n';
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
    // Если пользователь онлайн (есть его сокет), то отправляем сообщение ему
    if (this->users_sockets.find(message.receiver) != this->users_sockets.end())
    {
        std::cout << "Отправляем сообщение пользователю " << message.receiver << "..." << '\n';
        if (!TcpServer::send_msg(this->users_sockets[message.receiver], msg))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }

        // std::cout << "Пересылаем сообщение пользователю " << message.receiver << '\n';
    }
    // TODO: реализовать отправку сообщений пользователю, когда он не онлайн

    // Сохраняем сообщение в файл
    this->data_manager->save_msg(message);

    {
        std::stringstream ss;
        ss << message;
        this->logger->write(ss.str());
    }

    // Сохраняем сообщение в массив
    this->messages.push_back(std::move(message));

    return true;
}

void ChatServer::client_handler(int socket)
{
    std::cout << "Подключился клиент по сокету " << socket << '\n';

    std::string client_login;

    std::string msg;
    while (true)
    {
        // TODO: определять, что пользователь отключился
        msg.clear();
        if (!TcpServer::read_msg(socket, msg))
        {
            std::cout << "Клиент отключился." << '\n';
            break;
        }
        if (msg.empty())
        {
            // TODO: предусмотреть возможность повторной авторизации
            std::cout << "Похоже, что клиент отключился..." << '\n';
            this->users_sockets.erase(client_login);
            std::cout << "Удален сокет для пользователя " << client_login << '\n';
            return;
        }
        auto* net_msg = reinterpret_cast<sf::NetMessage*>(msg.data());

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
        else
        {
            // Если у этого пользователя не было сокета
            if (this->users_sockets.find(client_login) == this->users_sockets.end())
            {
                std::cout << "Для пользователя " << client_login << " сохранен сокет " << socket << '\n';
                this->users_sockets.emplace(client_login, socket);
            }
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
    }
}

void ChatServer::wait_for_stop()
{
    std::cout << "Нажмите \'q\' для остановки сервера" << '\n';
    std::string exit_msg;
    while (true)
    {
        std::cin >> exit_msg;
        if (exit_msg == "q")
        {
            std::cout << "Сервер отключается" << std::endl;
            break;
        }
    }
}

void ChatServer::run()
{
    // В отдельном потоке принимаем сокеты
    std::thread server_thread([&]() { this->accept_clients(); });
    server_thread.detach();
    // В основном потоке ждем команду для завершения программы
    this->wait_for_stop();
}

void print_os_info()
{
#if defined(__linux__)
    struct utsname utsname; // объект для структуры типа utsname

    uname(&utsname); // передаем объект по ссылке

    // распечатаем на экране информацию об операционной системе
    // эту информацию нам вернуло ядро Linux в объекте utsname
    std::cout << "OS name: " << utsname.sysname << std::endl;
    std::cout << "Host name: " << utsname.nodename << std::endl;
    std::cout << "OS release: " << utsname.release << std::endl;
    std::cout << "OS version: " << utsname.version << std::endl;
    std::cout << "Architecture: " << utsname.machine << std::endl;
#endif
}

int main(int argc, const char** argv)
{
    setlocale(LC_ALL, "");
    std::cout << "Чат запущен в ОС " << PLATFORM_NAME << std::endl;
    print_os_info();

    std::unique_ptr<IDataManager> data_manager =
        std::make_unique<FileDataManager>("chat_data/users.txt", "chat_data/messages.txt");
    std::unique_ptr<sf::ILogger> logger = std::make_unique<skillfactory::Logger>();
    ChatServer server(logger, data_manager);
    server.run();
    return 0;
}