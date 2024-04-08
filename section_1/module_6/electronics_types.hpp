#pragma once
#include "electronics.hpp"
#include <string>

class PortableElectronics: virtual public Electronics
{
    public:
        PortableElectronics(std::string manufacturer, std::string model, int battery_life);
        void show() override;
        void show_specs() override;
        virtual ~PortableElectronics();
    protected:
        int battery_life;
};

class HomeElectronics: virtual public Electronics
{
    public:
        HomeElectronics(std::string manufacturer, std::string model, float weight);
        void show() override;
        void show_specs() override;
        virtual ~HomeElectronics();
    protected:
        float weight;
};