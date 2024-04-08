#include "machine.hpp"
#include <limits>

Machine::Machine(): count(0), capacity(0)
{

}

Machine::Machine(int capacity): count(0)
{
    this->set_capacity(capacity);
    this->slots = new Slot*[this->capacity];
}

Machine::~Machine()
{
    delete[] slots;
}

Slot* Machine::operator[](int index)
{
    if(index < 0 || index >= this->capacity)
        return nullptr;
    return this->slots[index];
}

void Machine::add_slot(Slot* slot)
{
    // TODO: проверять указатель на пустоту
    if(this->count < this->capacity)
    {
        this->slots[this->count] = slot;
        this->count++;
    }
}

int Machine::get_capacity() const
{
    return this->capacity;
}

int Machine::get_empty_slots_count()
{
    return (this->capacity - this->count);
}

int Machine::get_snack_capacity()
{
    int res{0};
    for (size_t i = 0; i < this->get_capacity(); i++)
    {
        res += this->slots[i]->get_capacity();
    }
    return res;
    
}

std::ostream& operator<<(std::ostream& os, const Machine& machine)
{
    os << "Machine: " << std::endl;
    for (size_t i = 0; i < machine.count; i++)
    {
        os << i << ". " << *(machine.slots[i]) << std::endl;
    }
    return os;
}

void Machine::set_capacity(int capacity)
{
    if(capacity >= 0 && capacity < std::numeric_limits<int>::max())
        this->capacity = capacity;
    else
        this->capacity = 0;
}