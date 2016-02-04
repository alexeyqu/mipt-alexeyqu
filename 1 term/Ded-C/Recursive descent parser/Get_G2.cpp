/** ***************************************************************************
 * @file    Get_G2.cpp
******************************************************************************/

#include "RDP.h"

int Get_G2 (char* str)
{
    ASSERT (str);

    printf ("G2.\n");

    s = str;

    int res = Get_E_G2();

    GRAM_ASSERT (*s == 0);

    return res;
}

int Get_E_G2()
{
    printf ("Get_E_G2.\n");

    int val = Get_N_G2();

    while (*s == '+' || *s == '-')
    {
        int op = *s;

        s++;

        int val2 = Get_N_G2();

        if (op == '+') val += val2;
        if (op == '-') val -= val2;
    }

    return val;
}

int Get_N_G2()
{
    printf ("Get_N_G2.\n");

    int val = 0;

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
