#pragma once
#include <string>
#include "snack.hpp"
#include "slot.hpp"
#include <ostream>

class Machine
{
    public:
        Machine();
        Machine(int capacity);
        ~Machine();
        
        void add_slot(Slot* slot);
        int get_capacity() const;
        int get_empty_slots_count();

        int get_snack_capacity();

        Slot* operator[](int index);

        friend std::ostream& operator<<(std::ostream&, const Machine&);

    private:
        void set_capacity(int capacity);
        int capacity;    // максимальное количество слотов, помещаемых в автомат
        int count;   // количество слотов внутри слота
        Slot** slots;  // слоты внутри автомата
};