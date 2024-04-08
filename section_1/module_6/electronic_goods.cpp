#include "electronic_goods.hpp"
#include <iostream>


Refrigerator::Refrigerator(std::string manufacturer, std::string model, float weight, int volume):
HomeElectronics(manufacturer, model, weight), volume(volume)
{}

void Refrigerator::show_specs()
{
    HomeElectronics::show_specs();
    std::cout << " - объем: " << this->volume << "л" << std::endl;
}
Refrigerator::~Refrigerator(){}



PhotoCamera::PhotoCamera(std::string manufacturer, std::string model, int battery_life, float resolution):
PortableElectronics(manufacturer, model, battery_life), resolution(resolution)
{}

void PhotoCamera::show_specs()
{
    PortableElectronics::show_specs();
    std::cout << " - разрешение: " << this->resolution  << " мегапикселей" << std::endl;
}
PhotoCamera::~PhotoCamera(){}



Smartphone::Smartphone(std::string manufacturer, std::string model, int battery_life, float weight, std::string cellular):
PortableElectronics(manufacturer, model, battery_life), HomeElectronics(manufacturer, model, weight), cellular(cellular)
{}

void Smartphone::show()
{
    PortableElectronics::show();
}
void Smartphone::show_specs()
{
    PortableElectronics::show_specs();
    HomeElectronics::show_specs();
    std::cout << " - стандарт связи: " << this->cellular << std::endl;
}
Smartphone::~Smartphone(){}