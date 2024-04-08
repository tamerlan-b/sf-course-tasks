#pragma once
#include "electronics_types.hpp"

class Refrigerator final: public HomeElectronics
{
    public:
        Refrigerator(std::string manufacturer, std::string model, float weight, int volume);
        void show_specs() override;
        virtual ~Refrigerator();
    protected:
        int volume;
};

class PhotoCamera final: public PortableElectronics
{
    public:
        PhotoCamera(std::string manufacturer, std::string model, int battery_life, float resolution);
        void show_specs() override;
        virtual ~PhotoCamera();
    protected:
        float resolution;
};

class Smartphone final: public HomeElectronics, PortableElectronics
{
    public:
        Smartphone(std::string manufacturer, std::string model, int battery_life, float weight, std::string cellular);
        void show() override;
        void show_specs() override;
        virtual ~Smartphone();
    protected:
        std::string cellular;
};