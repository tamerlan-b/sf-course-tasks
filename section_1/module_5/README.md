# Простой вендинговый автомат

### Компиляция и запуск

1. С тестовыми данными:  
```bash
chmod +x run.sh
./run.sh
```

2. В режиме ручного ввода:  
```bash
g++ *.cpp -o module_5
./module_5
```

### Реализованный функционал
- Создание вендингового автомата
- Загрузка снэков в автомат
- Просмотр содержимого автомата
- Выбор и покупка снэка

### Структура классов
- `Snack`: снэк ([snack.hpp](snack.hpp), [snack.cpp](snack.cpp))
- `Slot`: слот для хранения снэков ([slot.hpp](slot.hpp), [slot.cpp](slot.cpp))
- `Machine`: автомат, хранящий множество слотов со снэками ([machine.hpp](machine.hpp), [machine.cpp](machine.cpp))
- `Application`: пользовательский интерфейс для работы с автоматом ([app.hpp](app.hpp), [app.cpp](app.cpp))