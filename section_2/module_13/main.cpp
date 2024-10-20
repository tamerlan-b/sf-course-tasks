#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "graph.hpp"

int main()
{
    Graph<std::string> g;
    // Оцифровываем граф
    g.add_vertex("Oleg");
    g.add_vertex("Nikita");
    g.add_vertex("Nastya");
    g.add_vertex("Vanya");
    g.add_vertex("Zhenya");
    g.add_vertex("Petya");
    g.add_vertex("Igor");
    g.add_vertex("Misha");
    g.add_vertex("Kostya");
    g.add_vertex("Sasha");
    g.add_vertex("Vlad");
    g.add_vertex("Stas");
    
    g.add_edge("Oleg", "Nikita", 1);
    g.add_edge("Nikita", "Nastya", 1);
    g.add_edge("Nastya", "Vanya", 1);
    g.add_edge("Vanya", "Zhenya", 1);

    g.add_edge("Nikita", "Igor", 1);
    g.add_edge("Igor", "Misha", 1);
    g.add_edge("Misha", "Kostya", 1);
    g.add_edge("Misha", "Sasha", 1);

    g.add_edge("Vanya", "Petya", 1);
    g.add_edge("Zhenya", "Vlad", 1);
    g.add_edge("Vlad", "Stas", 1);

    // Находим знакомства через 3 рукопожатия
    const int handshakes = 3;
    std::vector<std::tuple<std::string, std::string, int>> pairs;
    g.find_by_max_distance(handshakes, pairs);
    std::cout << "Acquaintance by no more than " << handshakes << " handshakes:" << std::endl;
    for (const auto [v1, v2, distance] : pairs)
    {
        std::cout << "\t" << v1 << " and " << v2 << ": " << distance << std::endl;
    }
    
    return 0;
}