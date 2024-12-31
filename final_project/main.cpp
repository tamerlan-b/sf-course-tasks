#include "app.hpp"
#include "data_manager_interface.hpp"
#include "file_data_manager.hpp"
#include <iostream>
#include <memory>

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

int main(int argc, char const* argv[])
{
    setlocale(LC_ALL, "");

    std::cout << "Чат запущен в ОС " << PLATFORM_NAME << std::endl;
    print_os_info();

    IDataManager* data_manager = new FileDataManager("chat_data/users.txt", "chat_data/messages.txt");
    App app(data_manager);
    app.run();
    delete data_manager;
    return 0;
}