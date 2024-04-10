#include <iostream>
#include <exception>
#include "integer_array.hpp"

void basic_test(IntegerArray& array)
{
    // Изменяем размер контейнера до 8
    array.resize(8);

    // Вставляем число 20 перед элементом с индексом 5
    array.insertBefore(20, 5);

    // Удаляем элемент с индексом 3
    array.remove(3);

    // Добавляем элементы 30 и 40 в конец и начало контейнера
    array.insertAtEnd(30);
    array.insertAtBeginning(40);

    // Проверяем копирование и присваивание
    {
        IntegerArray b{ array };
        b = array;
        b = b;
        array = array;
    }
}

void exception_test(IntegerArray& array)
{
    // Создание массива неправильного размера
    try
    {
        IntegerArray b(-10);
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    // Обращение по неправильному индексу
    try
    {
        array[-1] = 10;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    // Вставка по несуществующему индексу
    try
    {
        array.insertBefore(100, 256);
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }

    // Удаление по несуществующему индексу
    try
    {
        array.remove(-100);
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }
    
}

int main()
{
    // Обяъвляем контейнер с 10 элементами
    IntegerArray array(10);

    // Заполняем контейнер числами от 1 до 10
    for (int i{ 0 }; i<10; ++i)
        array[i] = i+1;

    // Тестируем обычную работу с контейнером
    basic_test(array);

    // Выводим содержимое контейнера
    for (int i{ 0 }; i<array.getLength(); ++i)
        std::cout << array[i] << ' ';
    std::cout << '\n';

    // Тестируем нештатные ситуации
    exception_test(array);

    return 0;
}