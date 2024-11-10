#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "trie.hpp"


std::ostream& operator<<(std::ostream& os, const std::vector<std::string>& v)
{
    for (const auto &item : v)
        os << item << "\n";
    return os;
}

/**
 * @brief Загружает словарь из файла
 * 
 * @param path_to_dict путь к файлу со словарем
 * @param dictionary загруженный словарь
 */
void load_dictionary(const std::string& path_to_dict, std::vector<std::string>& dictionary)
{
    std::string line;
    std::ifstream fs(path_to_dict);
    if(fs.is_open())
        while (std::getline(fs, line))
            dictionary.emplace_back(std::move(line));
    fs.close();
}

int main()
{
    // Загружаем словарь
    std::vector<std::string> dictionary;
    constexpr size_t max_dict_size = 380'000;
    dictionary.reserve(max_dict_size);
    load_dictionary("words_alpha.txt", dictionary);

    // Создаем префиксное дерево из считанного словаря
    trie::TrieNode* root = trie::create_trie(dictionary);

    // Максимальное количество слов, которые будут найдены
    constexpr int max_matches = 5;

    std::vector<std::string> matches;
    matches.reserve(max_matches);
    std::string input;
    while(1)
    {
        std::cout << "Search: ";
        std::cin >> input;
        // Ищем слова в префиксном дереве
        trie::find(root, input, matches, max_matches);
        std::cout << matches;
        matches.clear();
    }
    delete root;
    return 0;
}