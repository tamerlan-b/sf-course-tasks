# Магазин электроники

### Компиляция и запуск

1. С тестовыми данными:  
```bash
chmod +x run.sh
./run.sh
```

2. В режиме ручного ввода:  
```bash
g++ *.cpp -o module_6
./module_6
```

### Реализованный функционал
- Просмотр имеющихся товаров
- Просмотр спецификации конкретного товара
- Ромбовидное наследование ([`Smartphone`](electronic_goods.hpp))

### Структура проекта
- АБК: `Electronics` ([electronics.hpp](electronics.hpp), [electronics.cpp](electronics.cpp))
- дочерние классы портативной и бытовой электроники: `PortableElectronics`, `HomeElectronics` ([electronics_types.hpp](electronics_types.hpp), [electronics_types.cpp](electronics_types.cpp))
- конкретные виды дочерних классов: `Refrigerator`, `PhotoCamera`, `Smartphone` ([electronic_goods.hpp](electronic_goods.hpp), [electronic_goods.cpp](electronic_goods.cpp))
- пользовательский интерфейс для работы с автоматом: `Application` ([app.hpp](app.hpp), [app.cpp](app.cpp))