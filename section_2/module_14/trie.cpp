#include "trie.hpp"

#include <iostream>
#include <queue>

namespace trie
{
    void add_children(TrieNode* root, const std::string& word, size_t start_pos)
    {
        if(!root)
            return;
        if(start_pos >= word.size())
            return;
        TrieNode* current = root;
        for (size_t i = start_pos; i < word.size(); ++i)
        {
            // Создаем ноду
            TrieNode* node = new TrieNode();
            node->symbol = word[i];
            node->parent = current;
            // Добавляем её как child к current
            current->children.push_back(node);
            // Обновляем current на созданную родительскую ноду
            current = node;
        }
        current->is_eow = true;
    }

    void add_word(TrieNode* root, const std::string& word)
    {
        if(!root)
            return;
        TrieNode* current = root;
        for (size_t i = 0; i < word.size(); ++i)
        {
            // Находим дочерний узел с таким символом
            TrieNode* found_node = nullptr;
            auto ch_size = current->children.size();
            for (size_t j = 0; j < current->children.size(); ++j)
            {
                if(word[i] == current->children[j]->symbol)
                {
                    found_node = current->children[j];
                    break;
                }
            }

            if(found_node)
                current = found_node;
            else
                return add_children(current, word, i);
        }
        current->is_eow = true;
    }

    void print_trie(TrieNode* root, std::string offset)
    {
        if(!root)
            return;
        std::cout << offset << root->symbol  << '\n';
        for (int i = 0; i < root->children.size(); i++)
            print_trie(root->children[i], offset + "\t");
    }

    std::string get_word(const TrieNode* node)
    {
        if(!node)
            return "";
        const TrieNode* curr = node;
        std::string res = "";
        do
        {
            res = curr->symbol + res;
            curr = curr->parent;
        }
        while (curr->parent != nullptr);
        return res;
    }

    void find(const TrieNode* root, const std::string& str, std::vector<std::string>& matches, int num_matches)
    {
        if(!root)
            return;
        // Находим указатель на последний символ str
        int found_len = 0;
        const TrieNode* curr = root;
        for (size_t i = 0; i < str.size(); ++i)
        {
            for (const auto *child : curr->children)
            {
                if(child->symbol == str[i])
                {
                    curr = child;
                    ++found_len;
                }
            }
        }

        // Подстрока не найдена
        if(found_len != str.size())
            return;
        
        std::queue<const TrieNode*> queue;
        queue.push(curr);

        while (!queue.empty() && matches.size() < num_matches)
        {
            // Получаем первый элемент
            const TrieNode* top_item = queue.front();

            // Если он является концом слова, добавляем в matches
            if(top_item->is_eow)
            {
                auto my_str = get_word(top_item);
                matches.push_back(my_str);
            }

            // Удаляем элемент из очереди
            queue.pop();

            // Добавляем в очередь дочерние элементы
            for (const auto *child : top_item->children)
                queue.push(child);
        }
    }

    TrieNode* create_trie(const std::vector<std::string>& dictionary)
    {
        trie::TrieNode* root = new trie::TrieNode;
        for (size_t i = 0; i < dictionary.size(); ++i)
        {
            trie::add_word(root, dictionary[i]);
        }
        return root;
    }
}