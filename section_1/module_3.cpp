#include <iostream>
using namespace std;

enum Cmds
{
    ENCRYPT = 1,
    DECRYPT = 2,
    EXIT = 3
};

inline char encrypt(char c, int shift, char alpha_start, int alpha_len)
{
    return alpha_start + ((static_cast<unsigned int>(c - alpha_start) + shift) % alpha_len);
}

inline char decrypt(char c, int shift, char alpha_start, int alpha_len)
{
    int index = (static_cast<int>(c - alpha_start) - shift) % alpha_len;
    index = index < 0 ? index + alpha_len: index;
    return alpha_start + index;
}


int main(int argc, char const *argv[])
{
    setlocale (LC_ALL,"");
    
    
    while(true)
    {
        cout << "Выберите операцию:" << "\n";
        cout << "\t" << Cmds::ENCRYPT << ". Зашифровать строку" << "\n";
        cout << "\t" << Cmds::DECRYPT << ". Расшифровать строку" << "\n";
        cout << "\t" << Cmds::EXIT << ". Выход" << "\n";
        int operation;
        cin >> operation;
        if(operation == Cmds::EXIT)
        {
            cout << "Завершение работы" << "\n";
            break;
        }

        cout << "Введите длину строки:" << "\n";
        unsigned int str_len;
        cin >> str_len;
        cout << "Введите строку" << "\n";
        char input_str[256];
        cin >> input_str;
        input_str[str_len] = '\0';
        cout << "Введите сдвиг: ";
        unsigned int shift;
        cin >> shift;
        char output_str[str_len + 1];
        output_str[str_len] = '\0';


        switch (operation)
        {
            case Cmds::ENCRYPT:
            {
                char enc_char;
                for (size_t i = 0; i < str_len; i++)
                {
                    if(input_str[i] >= 'A' && input_str[i] <= 'Z')
                        enc_char = encrypt(input_str[i], shift, 'A', 26);
                    else if(input_str[i] >= 'a' && input_str[i] <= 'z')
                        enc_char = encrypt(input_str[i], shift, 'a', 26);
                    else
                        enc_char = input_str[i];
                    output_str[i] = enc_char;
                }

                cout << "Результат: " << output_str << "\n";
                break;
            }
            case Cmds::DECRYPT:
            {
                char dec_char;
                for (size_t i = 0; i < str_len; i++)
                {
                    if(input_str[i] >= 'A' && input_str[i] <= 'Z')
                        dec_char = decrypt(input_str[i], shift, 'A', 26);
                    else if(input_str[i] >= 'a' && input_str[i] <= 'z')
                        dec_char = decrypt(input_str[i], shift, 'a', 26);
                    else
                        dec_char = input_str[i];
                    output_str[i] = dec_char;
                }
                
                cout << "Результат: " << output_str << "\n";

                break;
            }
            default:
                break;
        }

    }
    
    
    // while (true)
    // {
    //     // Считываем переменные
    //     cout << "Введите операцию (поддерживаемые: +, -, *, /, !, ^)" << endl;
    //     cout << "Для выхода введите q" << endl;
    //     cin >> operation;

    //     // Выходим по q
    //     if(operation == 'q')
    //         break;

    //     // Считываем операнды
    //     cout << "Введите операнды для вычисления операции" << endl;
    //     cin >> a;
    //     if(operation != '!')
    //         cin >> b;
        
    //     // Обработка некорректного ввода
    //     if(cin.fail())
    //     {
    //         cout << "Ошибка ввода данных!" << endl;
    //         cin.clear();
    //         continue;
    //     }
    // }
    return 0;
}
