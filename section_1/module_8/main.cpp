#include <iostream>
#include "custom_array.h"

int main()
{
    using my_type = float;

    // Обяъвляем контейнер с 10 элементами
    CustomArray<my_type> array(10);

    // Заполняем контейнер числами от 1 до 10
    for (int i{ 0 }; i < 10; ++i)
        array[i] = static_cast<my_type>(i + 1);

    // Изменяем размер контейнера до 8
    array.resize(8);

    // Вставляем число 20.93 перед элементом с индексом 5
    array.insertBefore(static_cast<my_type>(20.93), 5);

    // Удаляем элемент с индексом 3
    array.remove(3);

    // Добавляем элементы 31.769 и 43.932 в конец и начало контейнера
    array.insertAtEnd(static_cast<my_type>(30.769));
    array.insertAtBeginning(static_cast<my_type>(40.932));

    // Проверяем копирование и присваивание
    {
        decltype(array) b{ array };
        b = array;
        b = b;
        array = array;
    }

    // Выводим содержимое контейнера
    for (int i{ 0 }; i<array.getLength(); ++i)
        std::cout << array[i] << ' ';
    std::cout << '\n';

    return 0;
}