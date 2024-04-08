#include <iostream>
#include "app.hpp"

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    Application app;
    app.run();
    return 0;
}
