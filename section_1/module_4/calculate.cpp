#include "calculate.h"

float sum(const float a, const float b)
{
    return a + b;
}

float subtract(const float a, const float b)
{
    return a - b;
}

float divide(const float a, const float b)
{
    return a / b;
}

float multiply(const float a, const float b)
{
    return a * b;
}

float pow(float num, int power)
{
    if(power == 1)
        return num;
    return num * pow(num, power - 1);
}

int factorial(int num)
{
    if(num == 1)
        return 1;
    return num * factorial(num - 1);
}
