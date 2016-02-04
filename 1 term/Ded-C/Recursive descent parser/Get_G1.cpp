/** ***************************************************************************
 * @file    Get_G1.cpp
******************************************************************************/

#include "RDP.h"

int Get_G1 (char* str)
{
    ASSERT (str);

    printf ("G1.\n");

    s = str;

    int res = Get_N_G1();

    GRAM_ASSERT (*s == 0);
    GRAM_ASSERT (res >= 0);

    return res;
}

int Get_N_G1()
{
    printf ("Get_N_G1.\n");

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
