#include "electronics_types.hpp"
#include <iostream>


PortableElectronics::PortableElectronics(std::string manufacturer, std::string model, int battery_life): battery_life(battery_life)
{
    this->manufacturer = manufacturer;
    this->model = model;
}

void PortableElectronics::show()
{
    std::cout << get_class_name(*this) << " " << this->manufacturer << " " << this->model << std::endl;
}

void PortableElectronics::show_specs()
{
    std::cout << " - время автономной работы: " << this->battery_life  << "ч" << std::endl;
}

PortableElectronics::~PortableElectronics(){}



HomeElectronics::HomeElectronics(std::string manufacturer, std::string model, float weight):  weight(weight)
{
    this->manufacturer = manufacturer;
    this->model = model;
}

void HomeElectronics::show()
{
    std::cout << get_class_name(*this) << " " << this->manufacturer << " " << this->model << std::endl;
}

void HomeElectronics::show_specs()
{
    std::cout << " - вес: " << this->weight << "кг" << std::endl;
}

HomeElectronics::~HomeElectronics(){}