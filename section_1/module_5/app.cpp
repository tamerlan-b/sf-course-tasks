#include "app.hpp"
#include "slot.hpp"
#include <iostream>
#include <string>
using namespace std;

#define TAB "\t"

enum Cmd
{
    CREATE=0,
    LOAD=1,
    SHOW=2,
    BUY=3,
    EXIT=4
};

Machine* Application::create_machine()
{
    cout << "Введите количество слотов в автомате:" << endl;
    int num_slots;
    cin >> num_slots;

    cout << "Введите размер 1-го слота:" << endl;
    int slot_capacity;
    cin >> slot_capacity;

    auto machine = new Machine(num_slots);
    for (size_t i = 0; i < machine->get_capacity(); i++)
    {
        auto slot = new Slot(slot_capacity);
        machine->add_slot(slot);
    }
    return machine;
}

Snack* Application::read_snack(int* count)
{
    cout << "Введи название снэка: ";
    std::string name;
    cin >> name;

    int calorie_content;
    cout << endl << "Введи калорийность: ";
    cin >> calorie_content;

    int price;
    cout << endl << "Введи стоимость: ";
    cin >> price;

    cout << endl << "Введи количество загружаемых снэков: ";
    cin >> *count;
    cout << endl;

    Snack* snack = new Snack(name, price, calorie_content);
    return snack;
}

void Application::fill_machine(Machine* machine)
{
    cout << "Выбери номер слота, в который хотите загрузить снеки" << endl;
    cout << "Доступны номера от 0 до " << machine->get_capacity() << endl;
    int slot_id;
    cin >> slot_id;
    if(slot_id < 0 || slot_id >= machine->get_capacity())
    {
        cout << "Неправильно выбран номер слота" << endl;
    }
    cout << "Выбран слот " << slot_id << "." << endl;
    Slot* selected_slot = (*machine)[slot_id];
    int slot_empty_count = selected_slot->get_empty_cell_count();
    if(slot_empty_count == 0)
    {
        cout << "Данный слот заполнен, попробуйте выбрать другой" << endl;
        return;
    }
    else
    {
        cout << "В него можно загрузить " << slot_empty_count << " снэков" << endl;
    }
    
    int snack_count(0);
    Snack* snack = read_snack(&snack_count);
    if(snack_count > slot_empty_count)
    {
        cout << "В слоте недостаточно места, чтобы загрузить все снэки" << endl;
        cout << "Загружаем только то, что помещается в него" << endl;
        for (size_t i = 0; i < slot_empty_count; i++)
        {
            selected_slot->add_snack(snack);
        }
        cout << "В слот загружено " << slot_empty_count << "/" << snack_count << " снэков" << endl;;
    }
    else
    {
        for (size_t i = 0; i < snack_count; i++)
        {
            selected_slot->add_snack(snack);
        }
        cout << "В слот загружены все снэки";
    }
}

void Application::show_content(const Machine* machine)
{
    cout << (*machine) << endl;
}

void Application::buy_snack(Machine* machine)
{
    cout << "Выбери номер слота с желаемым снеком" << endl;
    int slot_id;
    cin >> slot_id;
    if(slot_id < 0 || slot_id >= machine->get_capacity())
    {
        cout << "Нет такого слота!" << endl;
        return;
    }

    Slot* selected_slot = (*machine)[slot_id];
    int slot_snack_count = selected_slot->get_capacity() - selected_slot->get_empty_cell_count();
    if(slot_snack_count == 0)
    {
        cout << "В слоте нет снэков, попробуйте выбрать другой" << endl;
        return;
    }

    cout << "Выберите номер желаемого снэка" << endl;
    int snack_id;
    cin >> snack_id;
    if(snack_id < 0 || snack_id >= selected_slot->get_capacity())
    {
        cout << "Нет такого снэка!" << endl;
        return;
    }

    Snack snack = selected_slot->extract_snack(snack_id);
    if(snack.get_price() == 0 && snack.get_calorie_content() == 0)
    {
        cout << "Нет такого снэка!" << endl;
        return;
    }
    cout << "Выбран снэк " << snack.get_name() << ". С вас " << snack.get_price() << " рублей" << endl;

}

void Application::run()
{
    int operation;
    Machine* machine{nullptr};
    cout << "Привет! Я вендинговый автомат" << endl;
    
    while (true)
    {
        cout << "Выбери действие:" << endl;
        cout << TAB << Cmd::CREATE << ". Создать автомат" << endl;
        cout << TAB << Cmd::LOAD << ". Загрузить снеки" << endl;
        cout << TAB << Cmd::SHOW << ". Показать имеющиеся товары" << endl;
        cout << TAB << Cmd::BUY << ". Купить снэк" << endl;
        cout << TAB << Cmd::EXIT << ". Выход" << endl;

        cin >> operation;

        // TODO: везде проверять корректность ввода
        if(cin.fail())
        {
            cout << "К сожалению, ты ввел некорректное значение." << endl;
            cin.clear();
            break;
        }

        if(operation == Cmd::EXIT)
        {
            cout << "Пока!" << endl;
            break;
        }

        switch (operation)
        {
            case Cmd::CREATE:
                if(!machine)
                {
                    
                    machine = this->create_machine();
                    cout << "Создан автомат вместимостью " << machine->get_snack_capacity() << " снэков" << endl;
                }
                else
                {
                    cout << "Автомат уже создан" << endl;
                }
                break;
            case Cmd::LOAD:
                cout << "Загрузка снэков" << endl;
                this->fill_machine(machine);
                break;
            case Cmd::SHOW:
                cout << "Имеющиеся товары" << endl;
                this->show_content(machine);
                break;
            case Cmd::BUY:
                cout << "Покупка снэка" << endl;
                this->buy_snack(machine);
                break;
            default:
                cout << "Такой операции нет. Попробуй снова" << endl;
                break;
        }

    }
}