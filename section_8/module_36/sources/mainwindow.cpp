#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QStringListModel>
#include <QMessageBox>


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->users = {
        test::User("Alexey"),
        test::User("Ivan"),
        test::User("Mikhail"),
        test::User("Anna")
    };

    this->messages = {
        test::Message("Ivan", "Anna", "hello!"),
        test::Message("Anna", "Ivan", "hi!"),
        test::Message("Mikhail", "Alexey", "privet!"),
        test::Message("Alexey", "Mikhail", "privet! kak dela?")
    };

    this->showMessages();
    this->showUsers();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::showUsers()
{
    QStringList users_list;
    users_list.reserve(this->users.size());
    for (int i = 0; i < this->users.size(); ++i)
    {
        users_list.push_back(this->users[i].login.c_str());
    }
    // определяем модель
    QStringListModel *model = new QStringListModel(users_list);

    ui->usersListView->setModel(model);
}

void MainWindow::showMessages()
{
    QStandardItemModel* model=  new QStandardItemModel(this->messages.size(), 3);

    model->setHorizontalHeaderItem(0, new QStandardItem("sender"));
    model->setHorizontalHeaderItem(1, new QStandardItem("receiver"));
    model->setHorizontalHeaderItem(2, new QStandardItem("content"));
    for (int i = 0; i < this->messages.size(); ++i)
    {
        model->setItem(i, 0, new QStandardItem(this->messages[i].sender.c_str()));
        model->setItem(i, 1, new QStandardItem(this->messages[i].receiver.c_str()));
        model->setItem(i, 2, new QStandardItem(this->messages[i].content.c_str()));
    }

    ui->messagesTableView->setModel(model);
}

void MainWindow::on_disableButton_clicked()
{
    // TODO: нужно получить выбранного пользователя из списка
    auto index = this->ui->usersListView->currentIndex();
    // this->ui->usersListView->model()->
    QMessageBox::information(this, "Выбранный пользователь", index.data().toString());
}


void MainWindow::on_banButton_clicked()
{
    // TODO: нужно получить выбранного пользователя из списка
}

