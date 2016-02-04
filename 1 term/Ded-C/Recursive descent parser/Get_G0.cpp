/** ***************************************************************************
 * @file    Get_GO.cpp
******************************************************************************/

#include "RDP.h"

int Get_G0 (char* str)
{
    ASSERT (str);

    printf ("G0.\n");

    s = str;

    int res = Get_N_G0();

    GRAM_ASSERT (*s == 0);
    GRAM_ASSERT (res >= 0);

    return res;
}

int Get_N_G0()
{
    printf ("Get_N_G0.\n");

    int val = 0;

    if ('0' <= *s && *s <= '9')
    {
        val = *s - '0';
        s++;
    }

    return val;
}
