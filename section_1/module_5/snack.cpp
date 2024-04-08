#include "snack.hpp"
#include <limits>

Snack::Snack()
{}

Snack::Snack(std::string name, int price, int calorie_content)
{
    this->set_name(name);
    this->set_price(price);
    this->set_calorie_content(calorie_content);
}

std::ostream& operator<<(std::ostream& os, const Snack& obj)
{
    os << "name: " << obj.name << ", ";
    os << "price: " << obj.price << ", ";
    os << "calorie content: " << obj.calorie_content << "; ";
    return os;
}

std::string Snack::get_name()
{
    return this->name;
}

int Snack::get_calorie_content()
{
    return this->calorie_content;
}

int Snack::get_price()
{
    return this->price;
}

void Snack::set_name(std::string name)
{
    this->name = name;
}

void Snack::set_calorie_content(int calorie_content)
{
    if(calorie_content >= 0 && calorie_content < std::numeric_limits<int>::max())
        this->calorie_content = calorie_content;
}

void Snack::set_price(int price)
{
    if(price >= 0 && price < std::numeric_limits<int>::max())
        this->price = price;
}

Snack::~Snack(){}