#include <iostream>
using namespace std;

int main()
{
    setlocale (LC_ALL,"");
    cout << "Введите два числа" << endl;
    int a, b;
    cin >> a >> b;
    cout << a << " + " << b << " = " << a + b << endl;
    cout << a << " - " << b << " = " << a - b << endl;
    cout << a << " * " << b << " = " << a * b << endl;
    cout << a << " / " << b << " = " << static_cast<float>(a) / b << endl;
    cout << a << " & " << b << " = " << (a & b) << endl;
    cout << a << " | " << b << " = " << (a | b) << endl;
    cout << a << " ^ " << b << " = " << (a ^ b) << endl;
    return 0;
}