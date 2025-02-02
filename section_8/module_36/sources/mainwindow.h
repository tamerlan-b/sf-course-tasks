#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include <vector>

#include "chat_server.h"
#include "user.h"
#include "message.h"
#include <memory>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
\
    private slots:
    void on_disableButton_clicked();
    void on_banButton_clicked();

    private:
    Ui::MainWindow* ui;

    std::vector<User> users;
    std::vector<Message> messages;

    void showUsers();
    void showMessages();
    void showMessages(const std::vector<Message>& msgs);

    void onUsersUpdate(const std::vector<User>& users);
    void onMessagesUpdate(const std::vector<Message>& msgs);

  signals:
    void update_users_signal();
    void update_messages_signal();
  public slots:
    void update_users_slot();
    void update_messages_slot();

  private:
    std::shared_ptr<ChatServer> server;

    int find_user(const std::string& login) const noexcept;
};

#endif // MAINWINDOW_H
