#pragma once
#include <string>
#include "snack.hpp"
#include <ostream>

class Slot
{
    public:
        Slot();
        Slot(int capacity);
        ~Slot();
        
        void add_snack(Snack* snack);
        Snack extract_snack(int id);
        int get_capacity();
        int get_empty_cell_count();

        friend std::ostream& operator<<(std::ostream&, const Slot&);

    private:
        void remove(int id);
        void set_capacity(int capacity);
        int capacity;    // максимальное количество снэков, помещаемое в слот
        int count;   // количество снэков внутри слота
        Snack* snacks;  // снэки внутри слота
};