#pragma once
#include <string>

class Electronics
{
    public:
        virtual void show() = 0;
        virtual void show_specs() = 0;
        virtual ~Electronics() = default;
    protected:
        std::string manufacturer;
        std::string model;
};

template<typename T>
std::string get_class_name(T& obj)
{
    std::string full_name = typeid(obj).name();
    int start_index = 0;
    while(std::isdigit(full_name[start_index]))
    {
        start_index++;
    }
    return full_name.substr(start_index);
}