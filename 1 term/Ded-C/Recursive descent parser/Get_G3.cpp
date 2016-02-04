/** ***************************************************************************
 * @file    Get_G3.cpp
******************************************************************************/

#include "RDP.h"

double Get_G3 (char* str)
{
    ASSERT (str);

    printf ("G3.\n");

    s = str;

    double res = Get_E_G3();

    GRAM_ASSERT (*s == 0);

    return res;
}

double Get_E_G3()
{
    printf ("Get_E_G3.\n");

    double val = Get_T_G3();

    while (*s == '+' || *s == '-')
    {
        int op = *s;

        s++;

        double val2 = Get_T_G3();

        if (op == '+') val += val2;
        if (op == '-') val -= val2;
    }

    return val;
}

double Get_T_G3()
{
    printf ("Get_T_G3.\n");

    double val = Get_N_G3();

    while (*s == '*' || *s == '/')
    {
        int op = *s;

        s++;

        double val2 = Get_N_G3();

        if (op == '*') val *= val2;
        if (op == '/') val /= val2;
    }

    return val;
}

double Get_N_G3()
{
    printf ("Get_N_G3.\n");

    double val = 0;

    if (!('0' <= *s && *s <= '9'))
    {
        val = -1;

        return val;
    }

    do
    {
        val = val*10 + *s - '0';
        s++;
    } while ('0' <= *s && *s <= '9');

    return val;
}
