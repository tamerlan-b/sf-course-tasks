#include "app.hpp"
#include "electronics.hpp"
#include "electronic_goods.hpp"
#include <iostream>
#include <string>
using namespace std;

enum Cmd
{
    SHOW=1,
    SELECT=2,
    EXIT=3
};

void Application::fill_warehouse()
{
    this->electronics.push_back(new PhotoCamera("Sony", "ZV-1F", 450, 21.F));
    this->electronics.push_back(new Refrigerator("Indesit", "TS 4180 W", 65.4F, 298));
    this->electronics.push_back(new Refrigerator("Hitachi", "R-WX 630 KU XW", 125.F, 473));
    this->electronics.push_back(new Smartphone("Nokia", "6300", 348, 0.91F, "2G"));
    this->electronics.push_back(new Smartphone("Apple", "IPhone 15", 100, 0.171F, "5G"));
}

void Application::show_content()
{
    for (size_t i = 0; i < this->electronics.size(); i++)
    {
        cout << i + 1 << ". ";
        this->electronics[i]->show();
    }
}

void Application::show_product_spec()
{
    cout << "Выбери номер товара на складе: " << endl;
    int good_id;
    cin >> good_id;

    if(cin.fail())
    {
        cout << "К сожалению, ты ввел некорректное значение." << endl;
        cin.clear();
        return;
    }

    good_id--;
    if(good_id < 0 || good_id >= this->electronics.size())
    {
        cout << "Такого товара нет на складе :(" << endl;
        return;
    }

    cout << "Выбран товар ";
    this->electronics[good_id]->show();
    this->electronics[good_id]->show_specs();
}

void Application::run()
{
    this->fill_warehouse();
    int operation;
    cout << "Привет! Я склад электронной техники" << endl;
    while (true)
    {
        cout << "Выбери действие:" << endl;
        cout << "\t" << Cmd::SHOW << ". Показать содержимое склада" << endl;
        cout << "\t" << Cmd::SELECT << ". Показать спецификацию конкретного товара" << endl;
        cout << "\t" << Cmd::EXIT << ". Выход" << endl;

        cin >> operation;
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
            case Cmd::SHOW:
                cout << "Имеющиеся товары:" << endl;
                this->show_content();
                break;
            case Cmd::SELECT:
                this->show_product_spec();
                break;
            default:
                cout << "Такой операции нет. Попробуй снова" << endl;
                break;
        }

    }
}

Application::~Application()
{
    for (size_t i = 0; i < this->electronics.size(); i++)
    {
        delete this->electronics[i];
    }
}