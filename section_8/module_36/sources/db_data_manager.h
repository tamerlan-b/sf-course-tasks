#pragma once

#include "data_manager_interface.h"
#include "datetime.h"
#include "message.h"
#include "user.h"
#include <pqxx/pqxx>
#include <string>
#include <vector>

class DbDataManager : public IDataManager
{
  private:
    std::shared_ptr<pqxx::connection> conn;

  public:
    DbDataManager(std::string db_name, std::string user, std::string password)
    {
        std::string connection_options = std::string("dbname=") + db_name + " user=" + user + " password=" + password;
        this->conn = std::make_shared<pqxx::connection>(connection_options);
    }
    void save_user(User& user) override
    {
        pqxx::work tx{*this->conn};
        // Сохраняем пользователя в БД
        std::string insert_user_q = "INSERT INTO users (login) VALUES ('" + user.login + "') RETURNING id;";
        pqxx::result res = tx.exec(insert_user_q);
        // TODO: проверять, что пользователя не существует
        // Получаем id этого пользователя
        int user_id = res[0][0].as<int>();

        // Сохраняем хэш пароля в БД
        std::string insert_pass_q = "INSERT INTO passwords (user_id, pass_hash) VALUES ('" + std::to_string(user_id) +
                                    "', '" + user.pass_hash + "');";
        tx.exec(insert_pass_q);
        // Возвращаем id созданного пользователя
        tx.commit();
        user.id = user_id;
    };
    void load_users(std::vector<User>& users) const override
    {
        pqxx::work tx{*this->conn};
        for (auto [id, login, pass_hash] : tx.query<int, std::string, std::string>(
                 "SELECT users.id as id, users.login as login, passwords.pass_hash as hash "
                 "FROM users LEFT JOIN passwords ON users.id=passwords.user_id;"))
        {
            User user;
            user.id = id;
            user.login = std::move(login);
            user.pass_hash = std::move(pass_hash);
            users.push_back(std::move(user));
        }
    }

    void save_msg(Message& msg) override
    {
        pqxx::work tx{*this->conn};
        // Сохраняем сообщение в БД
        std::string insert_msg_q =
            "INSERT INTO messages (sender_id, reciever_id, content, created_at, condition) VALUES(" +
            std::to_string(msg.sender_id) + ", " + std::to_string(msg.receiver_id) + ", '" + msg.content + "', " + "'" +
            msg.created_at.to_string() + "', " + std::to_string(msg.condition) + ") RETURNING id;";
        pqxx::result res = tx.exec(insert_msg_q);
        tx.commit();
        int msg_id = res[0][0].as<int>();
        // Возвращаем id созданного сообщения
        msg.id = msg_id;
    }
    void load_msgs(std::vector<Message>& msgs) const override
    {
        pqxx::work tx{*this->conn};

        for (auto [id, sender_id, receiver_id, content, created_at, condition] :
             tx.query<int, int, int, std::string, std::string, int>("SELECT * FROM messages;"))
        {
            Message msg;
            msg.id = id;
            msg.sender_id = sender_id;
            msg.receiver_id = receiver_id;
            msg.content = std::move(content);
            msg.created_at = DateTime::parse(created_at);
            msg.condition = condition;
            msgs.push_back(std::move(msg));
        }
    }
    virtual ~DbDataManager(){};
};
