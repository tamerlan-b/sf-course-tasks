#pragma once
#include <string>
#include <vector>

/// @brief Алгоритмы для работы с префиксным деревом
namespace trie
{
    /// @brief Узел префиксного дерева
    struct TrieNode final
    {
        /// @brief символ, хранящийся в текущем узле
        char symbol;
        /// @brief указатель на родительский узел
        TrieNode* parent = nullptr;
        /// @brief Массив указателей, на дочерние узлы
        std::vector<TrieNode*> children;
        /// @brief флаг конца слова
        bool is_eow = false;
    };

    /**
     * @brief Рекурсивно добавляет дочерние узлы к узлу
     * префиксного дерева.
     * 
     * Функция добавляет подузлы без проверки их наличия в дереве
     * 
     * @param root узел префиксного дерева
     * @param word слово, буквы которого будут добавлены
     * @param start_pos индекс, начиная с которого будут добавляться буквы
     */
    void add_children(TrieNode* root, const std::string& word, size_t start_pos);
    
    /**
     * @brief Добавляет слово в префиксное дерево
     * 
     * @param root корневой узел дерева
     * @param word добавляемое слово
     */
    void add_word(TrieNode* root, const std::string& word);

    /**
     * @brief Рекурсивно выводит префиксное дерево в терминал
     * 
     * @param root корневой узел дерева
     * @param offset отступы дочерних элементов
     */
    void print_trie(TrieNode* root, std::string offset);

    /**
     * @brief Получает слово по узлу крайней буквы
     * 
     * @param node узел, содержащий крайнюю букву слова
     * @return std::string слово целиком
     */
    std::string get_word(const TrieNode* node);

    /**
     * @brief Находит подстроку в префиксном дереве
     * 
     * @param root корневой узел дерева
     * @param str искомая подстрока
     * @param matches массив строк совпадений
     * @param num_matches количество искомых совпадений, если -1, то найдет все имеющиеся в дереве
     */
    void find(const TrieNode* root, const std::string& str, std::vector<std::string>& matches, int num_matches = -1);

    /**
     * @brief Создает префиксное дерево из словаря
     * 
     * @param dictionary словарь строк
     * @return TrieNode* корневой узел дерева
     */
    TrieNode* create_trie(const std::vector<std::string>& dictionary);

}