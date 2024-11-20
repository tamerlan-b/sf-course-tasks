#include <iostream>
#include "app.hpp"

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    App app("chat_data/users.txt", "chat_data/messages.txt");
    app.run();
    return 0;
}