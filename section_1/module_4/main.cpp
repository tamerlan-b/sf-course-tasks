#include <iostream>
using namespace std;
#include "calculate.h"

int main()
{
    setlocale (LC_ALL,"");
    int a, b, result;
    char operation;
    while (true)
    {
        // Считываем переменные
        cout << "Введите операцию (поддерживаемые: +, -, *, /, !, ^)" << endl;
        cout << "Для выхода введите q" << endl;
        cin >> operation;

        // Выходим по q
        if(operation == 'q')
            break;

        // Считываем операнды
        cout << "Введите операнды для вычисления операции" << endl;
        cin >> a;
        if(operation != '!')
            cin >> b;
        
        // Обработка некорректного ввода
        if(cin.fail())
        {
            cout << "Ошибка ввода данных!" << endl;
            cin.clear();
            continue;
        }

        // Вычисляем значения операций
        switch (operation)
        {
            case '+':
                cout << sum(a, b) << endl;
                break;
            case '-':
                cout << subtract(a, b) << endl;
                break;
            case '*':
                cout << a * b << endl;
                break;
            case '/':
                if(b)
                {
                    cout << divide(a, b) << endl;
                }
                else
                {
                    cout << "На ноль делить нельзя" << endl;
                }
                break;
            case '!':
                cout << factorial(a) << endl;
                break;
            case '^':
                cout << pow(a, b) << endl;
                break;
            default:
                cout << "Операция '" << operation << "' отсутствует" << endl;
                break;
        }
    }
    return 0;
}