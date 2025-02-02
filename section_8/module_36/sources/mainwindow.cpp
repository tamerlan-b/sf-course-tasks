#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMessageBox>
#include "chat_server.h"
#include "db_data_manager.h"
#include "file_data_manager.h"
#include "logger.h"
#include "logger_interface.h"
namespace sf = skillfactory;


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Соединяем сигнал со слотом для отображения пользователей
    connect(this, SIGNAL(update_users_signal()), this, SLOT(update_users_slot()));

    // Соединяем сигнал со слотом для отображения сообщений
    connect(this, SIGNAL(update_messages_signal()), this, SLOT(update_messages_slot()));

    qDebug("Running server!");
    // std::unique_ptr<IDataManager> data_manager =
    //     std::make_unique<FileDataManager>("chat_data/users.txt", "chat_data/messages.txt");
    std::unique_ptr<IDataManager> data_manager = std::make_unique<DbDataManager>("sf_chat", "db_manager", "db_manager");
    std::unique_ptr<sf::ILogger> logger = std::make_unique<skillfactory::Logger>();
    this->server = std::make_shared<ChatServer>(logger, data_manager);
    this->server->run();

    qDebug("Server was launched!");

    // Подписываемся на событие отправки сообщений
    this->server->update_messages_event.subscribe([&](const std::vector<Message>& msgs){this->onMessagesUpdate(msgs);});

    // Подписываемся на событие обновления списка пользоваелей
    this->server->update_users_event.subscribe([&](const std::vector<User>& users){this->onUsersUpdate(users);});

    // Загружаем пользователей из сервера
    this->server->get_users(this->users);
    // Отображаем пользователей в строке
    this->showUsers();


    // Загружаем сообщения из сервера
    this->server->get_messages(this->messages);
    // Отображаем сообщения в таблице
    this->showMessages();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::onUsersUpdate(const std::vector<User>& users)
{
    emit update_users_signal();
}

void MainWindow::update_users_slot()
{
    // Загружаем обновленный список пользователей
    // TODO: получать только обновившихся пользователей
    this->users.clear();
    this->server->get_users(this->users);

    // Обновляем таблицу с пользователями
    this->showUsers();
}

void MainWindow::showUsers()
{
    // TODO: избежать пересоздания всей таблицы целиком
    QStringList users_list;
    users_list.reserve(this->users.size());
    for(const auto& user: this->users)
    {
        auto qstr = QString::fromStdString(user.login);
        users_list.push_back(std::move(qstr));
    }
    // определяем модель
    QStringListModel *model = new QStringListModel(users_list);

    this->ui->usersListView->setModel(model);
}


void MainWindow::onMessagesUpdate(const std::vector<Message>& msgs)
{
    // QMessageBox::information(this, "Info messages", "Messages callback was called!!!");
    emit update_messages_signal();
}

void MainWindow::update_messages_slot()
{
    // Загружаем обновленный список сообщений
    this->messages.clear();
    this->server->get_messages(this->messages);

    // Обновляем таблицу с сообщениями
    this->showMessages();
}

void MainWindow::showMessages()
{
    this->showMessages(this->messages);
}

void MainWindow::showMessages(const std::vector<Message>& msgs)
{
    std::vector<const char*> headers = {
        "sender_id",
        "receiver_id",
        "content",
        "created_at"
    };

    QStandardItemModel* model=  new QStandardItemModel(msgs.size(), headers.size());

    for (int i = 0; i < headers.size(); ++i) {
        model->setHorizontalHeaderItem(i, new QStandardItem(headers[i]));
    }

    // TODO: избежать пересоздания всей таблицы целиком
    for (int i = 0; i < msgs.size(); ++i)
    {
        const auto& msg = this->messages[i];
        model->setItem(i, 0, new QStandardItem(std::to_string(msg.sender_id).c_str()));
        model->setItem(i, 1, new QStandardItem(std::to_string(msg.receiver_id).c_str()));
        model->setItem(i, 2, new QStandardItem(msg.content.c_str()));
        model->setItem(i, 3, new QStandardItem(msg.created_at.to_string().c_str()));
    }

    ui->messagesTableView->setModel(model);
}

int MainWindow::find_user(const std::string& login) const noexcept
{
    for(const auto& u: this->users)
    {
        if(u.login == login)
        {
            return u.id;
        }
    }
    return -1;
}

void MainWindow::on_disableButton_clicked()
{
    auto index = this->ui->usersListView->currentIndex();
    const auto& login = index.data().toString().toStdString();

    // Поиск пользователя по логину
    int user_id = this->find_user(login);

    // TODO: отображать отключенных пользователей иначе
    this->server->disconnect_user(user_id);
    QString info = QString::fromStdString(std::string("Пользователь <b>" + login + "</b> был отключён"));
    QMessageBox::information(this, "Отключение", info);
}

void MainWindow::on_banButton_clicked()
{
    auto index = this->ui->usersListView->currentIndex();
    const auto& login = index.data().toString().toStdString();

    // Поиск пользователя по логину
    int user_id = this->find_user(login);

    // TODO: отображать забаненных пользователей иначе
    this->server->ban_user(user_id);
    QString info = QString::fromStdString(std::string("Пользователь <b>" + login + "</b> был забанен"));
    QMessageBox::information(this, "Бан", info);
}

