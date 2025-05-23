#include "chat_server.h"
#include "chat_msgs.h"
#include "db_data_manager.h"
#include "file_data_manager.h"
#include "logger.h"
#include "logger_interface.h"
#include "tcp_library.h"
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
    this->data_manager->load_users(this->users);
    this->data_manager->load_msgs(this->messages);

    // Заполняем статусы пользователей
    for(const auto&u: this->users)
    {
        this->users_statuses.emplace(u.id, UserStatus::NOT_CONNECTED);
    }

    cout << "Сервер успешно запущен!" << '\n';
}

int ChatServer::find_user(const std::string& login) const noexcept
{
    for (size_t i = 0; i < this->users.size(); ++i)
    {
        if (users[i].login == login)
        {
            return i;
        }
    }
    return -1;
}

int ChatServer::find_user(int id) const noexcept
{
    for (size_t i = 0; i < this->users.size(); ++i)
    {
        if (users[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

bool ChatServer::sign_in_handle(int socket, const std::string& msg, int& user_id)
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
    int user_index = this->find_user(login);
    // Проверяем, что хэш пароля совпадает
    bool is_correct_user = false;
    if (user_index >= 0)
    {
        is_correct_user = this->users[user_index].pass_hash == pass_hash;
    }
    // Если все ок, то сохраняем логин и отправляем пользователю OK
    sf::NetMessage resp_msg;
    resp_msg.type = sf::MsgType::SIGN_IN;
    if (is_correct_user)
    {
        std::cout << "Пользователь " << login << " авторизован, отправляем ответ" << '\n';
        resp_msg.status = sf::MsgStatus::OK;
        user_id = this->users[user_index].id;
    }
    else
    {
        std::cout << "Пользователь не найден" << '\n';
        // Иначе отправляем ошибку
        resp_msg.status = sf::MsgStatus::ERROR;
    }

    std::string response(reinterpret_cast<char*>(&resp_msg));
    if (!TcpServer::send_msg(socket, response))
    {
        std::cout << "Ошибка при отправке ответа" << '\n';
    }
    return is_correct_user;
}

bool ChatServer::sign_up_handle(int socket, const std::string& msg, int& user_id)
{
    // Считываем логин
    const auto* req_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    std::string login(req_msg->data);

    // Проверяем логин на занятость
    bool is_login_available = this->find_user(login) < 0;

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
    User user(login, pass_hash);
    user.id = this->users.size();
    this->data_manager->save_user(user);
    user_id = user.id;
    this->users.push_back(std::move(user));

    // Запускаем событие обновления списка пользователей
    this->update_users_event.happen(this->users);

    // Авторизуем пользователя (привязываем сокет к этому пользователю)
    {
        std::cout << "Пользователь " << user_id << " создан, отправляем ответ" << '\n';
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
    users_str += std::to_string(this->users.size()) + '\n';

    for (const auto& user : this->users)
    {
        users_str += std::to_string(user.id) + ' ' + user.login + '\n';
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

bool ChatServer::get_history_handle(int user_id, int socket)
{
    // Формируем сообщение
    std::stringstream sstr;

    // Показываем сообщения отправленные/адресованные текущему юзеру или адресованные всем
    for (const auto& msg : this->messages)
    {
        if (msg.sender_id == user_id || msg.receiver_id == user_id)
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

    std::cout << "Отправили список сообщений пользователю " << user_id << '\n';

    return true;
}

bool ChatServer::send_msg_handle(int user_id, int socket, const std::string& msg)
{
    // Распаковываем сообщение
    const auto* net_msg = reinterpret_cast<const sf::NetMessage*>(msg.data());
    Message message;
    {
        std::stringstream ss_msg;
        ss_msg << net_msg->data;
        ss_msg >> message;
    }
    message.sender_id = user_id; // Заполняем сами id отправителя
    // Если пользователя не существует, то отправляем ошибку
    if (this->find_user(message.receiver_id) < 0)
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
    if (this->users_sockets.find(message.receiver_id) != this->users_sockets.end())
    {
        std::cout << "Отправляем сообщение пользователю " << message.receiver_id << "..." << '\n';
        if (!TcpServer::send_msg(this->users_sockets[message.receiver_id], msg))
        {
            std::cout << "Ошибка при отправке ответа" << '\n';
        }

        // std::cout << "Пересылаем сообщение пользователю " << message.receiver << '\n';
    }
    // TODO: реализовать отправку сообщений пользователю, когда он не онлайн

    // FIXME: исправить баг
    if (message.condition < 0 || message.condition > 2)
    {
        message.condition = 0;
    }
    cout << "Message: " << message << '\n';
    // Сохраняем сообщение в файл
    this->data_manager->save_msg(message);

    {
        std::stringstream ss;
        ss << message;
        this->logger->write(ss.str());
    }

    // Сохраняем сообщение в массив
    this->messages.push_back(std::move(message));
    this->update_messages_event.happen(this->messages);

    return true;
}

void ChatServer::client_handler(int socket)
{
    std::cout << "Подключился клиент по сокету " << socket << '\n';

    int user_id = -1;

    std::string msg;
    while (true)
    {
        // TODO: определять, что пользователь отключился
        msg.clear();
        if (!TcpServer::read_msg(socket, msg))
        {
            std::cout << "Клиент отключился." << '\n';
            if(user_id >= 0)
            {
                this->users_statuses[user_id] = UserStatus::DISCONNECTED;
                this->users_sockets.erase(user_id);
            }
            break;
        }

        if (msg.empty())
        {
            // TODO: предусмотреть возможность повторной авторизации
            std::cout << "Похоже, что клиент отключился..." << '\n';
            if(user_id >= 0)
            {
                this->users_statuses[user_id] = UserStatus::DISCONNECTED;
                this->users_sockets.erase(user_id);
            }
            std::cout << "Удален сокет для пользователя " << user_id << '\n';
            return;
        }
        auto* net_msg = reinterpret_cast<sf::NetMessage*>(msg.data());

        switch (net_msg->type)
        {
            case sf::MsgType::SIGN_IN:
                {
                    std::cout << "SIGN_IN" << '\n';
                    this->sign_in_handle(socket, msg, user_id);
                    break;
                }
            case sf::MsgType::SIGN_UP:
                {
                    std::cout << "SIGN_UP" << '\n';
                    this->sign_up_handle(socket, msg, user_id);
                    break;
                }
            default:
                break;
        }

        if (user_id < 0)
        {
            continue;
        }
        else
        {
            if(this->users_statuses[user_id] == UserStatus::BANNED)
            {
                std::cout << "Забаненный пользователь " << user_id << " попытался войти в чат!" << '\n';
                return;
            }
            // TODO: проверять пользователя на наличие бана
            // Если у этого пользователя не было сокета
            if (this->users_sockets.find(user_id) == this->users_sockets.end())
            {
                std::cout << "Для пользователя " << user_id << " сохранен сокет " << socket << '\n';
                this->users_sockets.emplace(user_id, socket);
                // Запоминаем статус пользователя
                this->users_statuses[user_id] = UserStatus::CONNECTED;
            }
        }

        // Прерываем выполнение потока
        if(this->users_statuses[user_id] == UserStatus::DISCONNECTED)
        {
            this->users_sockets.erase(user_id);
            std::cout << "Клиент " << user_id << " с сокетом " << socket << " был отключен" << '\n';
            return;
        }

        switch (net_msg->type)
        {
            case sf::MsgType::SIGN_IN:
            case sf::MsgType::SIGN_UP:
                break;
            case sf::MsgType::GET_HISTORY:
                {
                    std::cout << "GET_HISTORY" << '\n';
                    this->get_history_handle(user_id, socket);
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
                    this->send_msg_handle(user_id, socket, msg);
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
        std::cout << "Get client" << '\n';
        if (sock)
        {
            std::function<void()> f = [&]() { this->client_handler(*sock); };
            this->socket_threads.emplace(*sock, std::move(f));
            this->socket_threads[*sock].detach();
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
    // this->wait_for_stop();
}

void ChatServer::get_users(std::vector<User>& users) const noexcept
{
    users.reserve(this->users.size());
    for(const auto&u: this->users)
    {
        users.emplace_back(u);
    }
}

void ChatServer::get_messages(std::vector<Message>& msgs) const noexcept
{
    msgs.reserve(this->messages.size());
    for(const auto& msg: this->messages)
    {
        msgs.emplace_back(msg);
    }
}

bool ChatServer::set_user_status(int user_id, UserStatus status)
{
    if(this->users_statuses.find(user_id) == this->users_statuses.end())
    {
        return false;
    }
    this->users_statuses[user_id] = status;
    return true;
}

bool ChatServer::disconnect_user(int user_id)
{
    return this->set_user_status(user_id, UserStatus::DISCONNECTED);
}

bool ChatServer::ban_user(int user_id)
{
    return this->set_user_status(user_id, UserStatus::BANNED);
}

ChatServer::~ChatServer()
{
    // std::cout << "Call server destructor" << '\n';
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

// int main(int argc, const char** argv)
// {
//     setlocale(LC_ALL, "");
//     std::cout << "Чат запущен в ОС " << PLATFORM_NAME << std::endl;
//     print_os_info();

//     // std::unique_ptr<IDataManager> data_manager =
//     //     std::make_unique<FileDataManager>("chat_data/users.txt", "chat_data/messages.txt");
//     std::unique_ptr<IDataManager> data_manager = std::make_unique<DbDataManager>("sf_chat", "db_manager", "db_manager");
//     std::unique_ptr<sf::ILogger> logger = std::make_unique<skillfactory::Logger>();
//     ChatServer server(logger, data_manager);
//     server.run();
//     return 0;
// }
