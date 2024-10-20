#ifndef __GRAPH__
#define __GRAPH__

#define SIZE 20
#define VERYBIGINT 1000000000

#include <vector>
#include <tuple>

template<typename T>
class Graph
{
    public:
        /**
         * @brief Конструктор по умолчанию.
         */
        Graph()
        {
            for(int i = 0; i < SIZE; ++i)
                for(int j = 0; j < SIZE; ++j)
                    this->matrix[i][j] = 0;
            this->vertex_count = 0;
        }
        
        /**
         * @brief Добавление вершины
         * 
         * @param vertex вершина
         */
        void add_vertex(T vertex)
        {
            if(this->find_vertex(vertex) < 0)
            {
                this->vertices[this->vertex_count] = vertex;
                this->vertex_count++;
            }
        }
        
        /**
         * @brief Добавление ребра
         * 
         * @param v1 вершина 1
         * @param v2 вершина 2
         * @param weight вес ребра
         */
        void add_edge(T v1, T v2, int weight)
        {
            this->set_edge(v1, v2, weight);
        }
        
        /**
         * @brief Поиск всех минимальных путей алгоритмом Флойда-Уоршелла
         * Подробнее: https://neerc.ifmo.ru/wiki/index.php?title=%D0%90%D0%BB%D0%B3%D0%BE%D1%80%D0%B8%D1%82%D0%BC_%D0%A4%D0%BB%D0%BE%D0%B9%D0%B4%D0%B0
         */
        void find_min_distances_Floyd()
        {
            this->init_distances();
            
            for(int k = 0; k < this->vertex_count; ++k) // итерации по k
            {
                for(int i = 0; i < this->vertex_count; ++i)
                {
                    if(i == k)
                        continue;
                    for(int j = 0; j < this->vertex_count; ++j)
                    {
                        if(j == k)
                            continue;
                        if(this->distances[i][k] + this->distances[k][j] < this->distances[i][j])
                        {
                            // пересчет матрицы путей
                            this->distances[i][j] = this->distances[i][k] + this->distances[k][j];
                        }
                    }
                }
            }
        }

        /**
         * @brief Находит вершины, минимальное расстояние между которыми
         * не превышает max_distance
         * 
         * @param max_distance наибольшее допустимое расстояние между вершинами
         * @param[out] pairs вектор с парами вершин и минимальными расстояниями
         */
        void find_by_max_distance(
            int max_distance,
            std::vector<std::tuple<T,T, int>>& pairs)
        {
            // Находим все минимальные расстояния между вершинами
            this->find_min_distances_Floyd();
            // Среди них находим нужные нам значения
            for (size_t i = 0; i < this->vertex_count; ++i)
                for (size_t j = i + 1; j < this->vertex_count; ++j)
                    if(this->distances[i][j] <= max_distance)
                        pairs.emplace_back(
                            this->vertices[i], 
                            this->vertices[j], 
                            this->distances[i][j]);
        }
    private:
        
        /**
         * @brief Проверка ребра на существование
         * 
         * @param v1_id индекс 1-ой вершины
         * @param v2_id индекс 2-ой вершины
         * @return true ребро существует
         * @return false ребро не существует
         */
        bool edge_exists(int v1_id, int v2_id) const
        {
            if(v1_id < 0 || v1_id >= SIZE)
                throw std::out_of_range();
            if(v2_id < 0 || v2_id >= SIZE)
                throw std::out_of_range();
            return matrix[v1_id][v2_id] > 0;
        }

        int get_edge(T v1, T v2) const
        {
            int v1_id = this->find_vertex(v1);
            if(v1_id < 0)
                throw std::out_of_range("There is no vertex v1!");
            int v2_id = this->find_vertex(v2);
            if(v2_id < 0)
                throw std::out_of_range("There is no vertex v2!");
            return this->matrix[v1_id][v2_id];
        }

        /**
         * @brief Задает ребро между вершинами
         * 
         * @param v1 вершина 1
         * @param v2 вершина 2
         * @param weight вес ребра
         */
        void set_edge(const T& v1, const T& v2, int weight)
        {
            int v1_id = this->find_vertex(v1);
            if(v1_id < 0)
                throw std::out_of_range("There is no vertex v1!");
            int v2_id = this->find_vertex(v2);
            if(v2_id < 0)
                throw std::out_of_range("There is no vertex v2!");
            this->matrix[v1_id][v2_id] = weight;
            this->matrix[v2_id][v1_id] = weight;
        }

        /**
         * @brief Находит индекс вершины в массиве
         * 
         * @param vertex вершина
         * @return int индекс вершины в массиве, если её нет, то -1
         */
        int find_vertex(const T& vertex) const noexcept
        {
            for(int i = 0; i < this->vertex_count; ++i)
                if(vertices[i] == vertex)
                    return i;
            return -1;
        }

        /**
         * @brief Инициализирует массив расстояний между вершинами
         * 
         */
        void init_distances()
        {
            for(int i = 0; i < SIZE; ++i)
                for(int j = 0; j < SIZE; ++j)
                    this->distances[i][j] = i==j ? 0 : edge_exists(i,j) ? matrix[i][j] : VERYBIGINT;
        }

        /// @brief Матрица смежности
        int matrix[SIZE][SIZE];
        
        /// @brief Матрица минимальных расстояний между вершинами
        int distances[SIZE][SIZE];
        
        /// @brief Вершины графа
        T vertices[SIZE];
        
        /// @brief Количество добавленных вершин
        int vertex_count;
};
#endif