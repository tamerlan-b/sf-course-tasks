#pragma once
#include <cstddef>
#include <vector>

class ArraySum
{
  public:
    ArraySum(int size, int min_value, int max_value);
    int sequential_sum();
    int parallel_sum(int block_size);

  private:
    void generate_random_array(int size, int min_value, int max_value);
    static void partial_sum(const std::vector<int>& arr, size_t start_index, size_t stop_index, int& sum);

    std::vector<int> arr;
};