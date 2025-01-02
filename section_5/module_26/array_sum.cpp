#include "array_sum.hpp"
#include <random>
#include <thread>

ArraySum::ArraySum(int size, int min_value, int max_value) { this->generate_random_array(size, min_value, max_value); }

int ArraySum::sequential_sum()
{
    int sum = 0;
    ArraySum::partial_sum(this->arr, 0, this->arr.size(), sum);
    return sum;
}

int ArraySum::parallel_sum(int block_size)
{
    // Создаем массив с потоками
    std::vector<std::thread> threads;
    threads.reserve(block_size);

    int sum = 0;
    int num_blocks = this->arr.size() / block_size;
    for (size_t i = 0; i < num_blocks; ++i)
    {
        threads.emplace_back(partial_sum, std::ref(this->arr), i * block_size, i * block_size + block_size,
                             std::ref(sum));
    }

    int res_len = this->arr.size() % block_size;
    if (res_len != 0)
    {
        threads.emplace_back(partial_sum, std::ref(this->arr), num_blocks * block_size,
                             num_blocks * block_size + res_len, std::ref(sum));
    }

    // Ожидаем завершения всех потоков
    for (auto& thr : threads)
    {
        if (thr.joinable())
        {
            thr.join();
        }
    }
    return sum;
}

void ArraySum::generate_random_array(int size, int min_value, int max_value)
{
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> my_dist(min_value, max_value);
    this->arr.reserve(size);

    for (size_t i = 0; i < size; ++i)
    {
        this->arr.push_back(my_dist(rng));
    }
}

void ArraySum::partial_sum(const std::vector<int>& arr, size_t start_index, size_t stop_index, int& sum)
{
    int local_sum = 0;
    for (size_t i = start_index; i < stop_index; ++i)
    {
        local_sum += arr[i];
    }
    sum += local_sum;
}