#include "slot.hpp"
#include <limits>

Slot::Slot(): count(0), capacity(0)
{

}

Slot::Slot(int capacity): count(0)
{
    this->set_capacity(capacity);
    this->snacks = new Snack[this->capacity];
}

Slot::~Slot()
{
    delete[] this->snacks;
}

std::ostream& operator<<(std::ostream& os, const Slot& slot)
{
    os << "Slot: " << std::endl;
    for (size_t i = 0; i < slot.count; i++)
    {
        os << "\t" << i << ". " << slot.snacks[i] << std::endl;
    }
    return os;
}

void Slot::add_snack(Snack* snack)
{
    if(this->count < this->capacity)
    {
        this->snacks[this->count] = *snack;
        this->count++;
    }
}

void Slot::remove(int id)
{
    for (size_t i = id; i < count - 1; i++)
    {
        this->snacks[i] = this->snacks[i + 1];
    }
    this->count--;
    
}

Snack Slot::extract_snack(int id)
{
    if(id < 0 || id >= this->count)
    {
        return Snack("EMPTY", 0, 0);
    }
    Snack res = this->snacks[id];
    this->remove(id);
    return res;
}

int Slot::get_capacity()
{
    return this->capacity;
}

int Slot::get_empty_cell_count()
{
    return (this->capacity - this->count);
}

void Slot::set_capacity(int capacity)
{
    if(capacity >= 0 && capacity < std::numeric_limits<int>::max())
        this->capacity = capacity;
    else
        this->capacity = 0;
}