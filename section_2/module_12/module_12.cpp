#include <iostream>
#include <string>
#include <cassert>

bool IsKPeriodic(const std::string& str, int k)
{
    // Если строка кратна k, то её длина должна быть k * m
    if(str.size() % k > 0)
        return false;
    // Проверяем, что подстрока повторяется
    for (int i = k; i < str.size(); i+=k)
        for (int j = 0; j < k; j++)
            if(str[i + j] != str[j])
                return false;
    return true;
}

struct TestValues{
    std::string str;
    int k;
    bool res;
    
    TestValues(std::string str, int k, bool res): str(str), k(k), res(res) {}
};


int main()
{
    TestValues test_cases[] = 
    {
      {"abcabcabcabc", 3, true},
      {"abcabcabcabca", 3, false},
      {"abcdefg", 7, true},
      {"abcdefg", 6, false},
      {"a", 1, true},
      {"bbbbbbb", 1, true},
      {"bbbbbb", 2, true},
      {"abcab", 3, false},
    };
    
    for (const auto& tc :test_cases)
    {
        bool my_res = IsKPeriodic(tc.str, tc.k);
        std::cout << "str: " << tc.str;
        std::cout << " k: " << tc.k;
        std::cout << " res (exp|got): " << tc.res; 
        std::cout << "|" << my_res << "\n";
        assert((my_res == tc.res) && "Failed! Values should be equal");
    }
    
    return 0;
}