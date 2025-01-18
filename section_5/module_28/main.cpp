#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>

bool make_thread = true;

// Слияние отсортированных подмассивов
void merge(int* arr, int l, int m, int r)
{
    int nl = m - l + 1;
    int nr = r - m;

    // создаем временные массивы
    int left[nl], right[nr];

    // копируем данные во временные массивы
    for (int i = 0; i < nl; i++)
        left[i] = arr[l + i];
    for (int j = 0; j < nr; j++)
        right[j] = arr[m + 1 + j];

    int i = 0, j = 0;
    int k = l; // начало левой части

    while (i < nl && j < nr)
    {
        // записываем минимальные элементы обратно во входной массив
        if (left[i] <= right[j])
        {
            arr[k] = left[i];
            i++;
        }
        else
        {
            arr[k] = right[j];
            j++;
        }
        k++;
    }
    // записываем оставшиеся элементы левой части
    while (i < nl)
    {
        arr[k] = left[i];
        i++;
        k++;
    }
    // записываем оставшиеся элементы правой части
    while (j < nr)
    {
        arr[k] = right[j];
        j++;
        k++;
    }
}

// Сортировка слиянием
void mergeSort(int* arr, int l, int r)
{
    if (l >= r)
    {
        return;
    }

    int m = (l + r - 1) / 2;

    // Выполняем в отдельном потоке
    if (make_thread && (m - l > 10000))
    {
        auto f = std::async(std::launch::async, [&]() { mergeSort(arr, l, m); });
        mergeSort(arr, m + 1, r);
    }
    else // Выполняем синхронно
    {

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
    }
    merge(arr, l, m, r);
}

// Заполнение массива случайными значениями
void fill_random(int* arr, long len)
{
    for (long i = 0; i < len; ++i)
    {
        arr[i] = std::rand() % 500000;
    }
}

// Класс для замера времени
class MyTimer
{
  private:
    std::chrono::time_point<std::chrono::steady_clock> start;

  public:
    MyTimer() { start = std::chrono::steady_clock::now(); }
    ~MyTimer()
    {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = end - start;
        std::cout << "Elapsed time: " << diff.count() << " seconds" << '\n';
    }
};

int main(int argc, const char** argv)
{
    std::srand(0);
    const long len = 1000000;
    int* arr = new int[len];

    std::cout << "Merge sort on array with " << len << " elements" << '\n';

    fill_random(arr, len);
    std::cout << "Sequential version:" << '\n';
    make_thread = false;
    {
        MyTimer timer;
        mergeSort(arr, 0, len);
    }

    fill_random(arr, len);
    std::cout << "Parallel version:" << '\n';
    make_thread = true;
    {
        MyTimer timer;
        mergeSort(arr, 0, len);
    }
    delete[] arr;
    return 0;
}