#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include <vector>

namespace Ui
{
    class MainWindow;
}

namespace test
{
    class Message
    {
      public:
        std::string sender;
        std::string receiver;
        std::string content;

        Message(std::string sender, std::string receiver, std::string content):
            sender(sender), receiver(receiver), content(content){}
    };

    class User
    {
      public:
        int id;
        std::string login;
        User(std::string login): login(login)
        {

        }
    };
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

    std::vector<test::User> users;
    std::vector<test::Message> messages;

    void showUsers();
    void showMessages();
};

#endif // MAINWINDOW_H
