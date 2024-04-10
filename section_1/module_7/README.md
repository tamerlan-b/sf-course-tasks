# Контейнер данных IntegerArray

### Компиляция и запуск

1. С тестовыми данными:  
```bash
chmod +x run.sh
./run.sh
```

2. В режиме ручного ввода:  
```bash
g++ *.cpp -o module_7
./module_7
```

### Реализованный функционал
- создание контейнера;
- копирование контейнера;
- доступ к любому элементу контейнера по индексу;
- изменение размера контейнера;
- вставка элемента в контейнер;
- удаление элемента из контейнера.

### Структура проекта
- контейнер: `IntegerArray` ([integer_array.hpp](integer_array.hpp), [integer_array.cpp](integer_array.cpp))
- собственные типы исключений: `BadRange`, `BadLength` ([bad_range.hpp](bad_range.hpp), [bad_length.cpp](bad_length.cpp))