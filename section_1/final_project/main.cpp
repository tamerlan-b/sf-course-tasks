#include <iostream>
#include "app.hpp"

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    App app("chat_data/users.json", "chat_data/messages.json");
    app.run();
    return 0;
}