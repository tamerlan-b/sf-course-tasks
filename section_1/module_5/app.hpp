#pragma once

#include "machine.hpp"
#include "snack.hpp"

class Application
{
    public:
        Application(){}
        ~Application(){}
        void run();
    private:
        Machine* create_machine();
        Snack* read_snack(int* count);
        void fill_machine(Machine* machine);
        void show_content(const Machine* machine);
        void buy_snack(Machine* machine);
};