/** ***************************************************************************
 * @file    Get_G4.cpp
******************************************************************************/

#include "RDP.h"

double Get_G4 (char* str)
{
    ASSERT (str);

    printf ("G4.\n");

    s = str;

    double res = Get_E_G4();

    GRAM_ASSERT (*s == 0);

    return res;
}

double Get_E_G4()
{
    printf ("Get_E_G4.\n");

    double val = Get_T_G4();

    while (*s == '+' || *s == '-')
    {
        int op = *s;

        s++;

        double val2 = Get_T_G4();

        if (op == '+') val += val2;
        if (op == '-') val -= val2;
    }

    return val;
}

double Get_T_G4()
{
    printf ("Get_T_G4.\n");

    double val = Get_P_G4();

    while (*s == '*' || *s == '/')
    {
        int op = *s;

        s++;

        double val2 = Get_P_G4();

        if (op == '*') val *= val2;
        if (op == '/') val /= val2;
    }

    return val;
}

double Get_P_G4()
{
    printf ("Get_P_G4.\n");

    double val = 0;

    if (*s == '-')
    {
        val = -1;
        s++;
    }
    else
    {
        if (*s == '+')
        {
            s++;
        }

        val = 1;
    }

    if (*s == '(')
    {
        s++;

        val *= Get_E_G4();

        GRAM_ASSERT (*s == ')');

        s++;

        return val;
    }
    else
    {
        val *= Get_N_G4();

        return val;
    }
}

double Get_N_G4()
{
    printf ("Get_N_G4.\n");

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
