#pragma once
#include "electronics.hpp"
#include <vector>

class Application final
{
    public:
        Application(){}
        virtual ~Application();
        void run();
    private:
        std::vector<Electronics*> electronics;
        void fill_warehouse();
        void show_content();
        void show_product_spec();
};