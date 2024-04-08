#pragma once
#include <string>
#include <ostream>

class Snack
{
    public:
        Snack();
        Snack(std::string name, int price, int calorie_content);
        ~Snack();

        friend std::ostream& operator<<(std::ostream&, const Snack&);

        std::string get_name();
        int get_calorie_content();
        int get_price();

        void set_name(std::string name);
        void set_calorie_content(int calorie_content);
        void set_price(int price);
    private:
        int calorie_content;    // калорийность
        int price;    // цена
        std::string name;   // название
};