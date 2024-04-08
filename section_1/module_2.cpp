#include <iostream>
using namespace std;

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
                cout << a + b << endl;
                break;
            case '-':
                cout << a - b << endl;
                break;
            case '*':
                cout << a * b << endl;
                break;
            case '/':
                if(b)
                {
                    cout << a / static_cast<float>(b) << endl;
                }
                else
                {
                    cout << "На ноль делить нельзя" << endl;
                }
                break;
            case '!':
                result = 1;
                while(a > 0)
                {
                    result *= a--;
                }
                cout << result << endl;
                break;
            case '^':
                result = 1;
                while (b > 0)
                {
                    result *= a;
                    b--;
                }
                
                cout << result << endl;
                break;
            default:
                cout << "Операция '" << operation << "' отсутствует" << endl;
                break;
        }
    }
    return 0;
}