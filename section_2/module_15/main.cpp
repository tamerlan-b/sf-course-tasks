#include "hashtable.hpp"
#include <iostream>
namespace sf = skillfactory;

size_t hash_func(std::string fr_name)
{
    size_t sum = 0;
    for(size_t i = 0; i < fr_name.size(); ++i) {
        sum += fr_name[i];
    }
    return sum;
}

int main(int argc, char const *argv[])
{
    std::function<size_t(std::string)> hf = hash_func;
    sf::HashTable<std::string, size_t> ht(hf);
    ht.add("apple", 3);
    ht.add("banana", 5);
    ht.add("pineapple", 1);
    ht.add("lemon", 4);
    ht.add("watermelom", 1);
    ht.add("kiwi", 9);
    ht.add("plum", 12);
    ht.add("tangerie", 18);
    ht.add("orange", 6);
    ht.remove("apple");
    ht.remove("orange");
    ht.remove("kiwi");
    std::cout << ht << std::endl;

    auto find_res = ht.find("plum");
    std::cout << "Find plum: " << (find_res ? static_cast<int>(find_res.value()) : -1) << std::endl;
    return 0;
}
