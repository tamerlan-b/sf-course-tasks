#include "array_sum.hpp"
#include <chrono>
#include <functional>
#include <iostream>

double measure_time(const std::function<void()>& func)
{
    auto t_start = std::chrono::high_resolution_clock::now();
    func();
    // calc_sum(arr, N, seq_sum);
    auto t_end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t_end - t_start).count();
}

int main()
{
    const int N = 1000; // размер массива
    const int M = 4;    // размер подмассива для параллельного вычисления
    const int min_val = 1;
    const int max_val = 100;

    ArraySum ar_sum(N, min_val, max_val);

    int seq_sum;
    double seq_time_ms = measure_time([&seq_sum, &ar_sum]() { seq_sum = ar_sum.sequential_sum(); });
    std::cout << "Sequential sum: " << seq_sum << " for " << seq_time_ms << " ms " << "\n";

    int par_sum;
    double par_time_ms = measure_time([&ar_sum, &par_sum, M]() { par_sum = ar_sum.parallel_sum(M); });
    std::cout << "Parallel sum: " << par_sum << " for " << par_time_ms << " ms " << "\n";

    return 0;
}