/** ***************************************************************************
 * @file    Misc.cpp
******************************************************************************/

#include "RDP.h"

int Get_String (FILE* instream, char* str)
{
    ASSERT (instream); ASSERT (str);

    printf ("Getting string.\n");

    int c = 0;

    while(1)
    {
        c = fgetc (instream);

        if (c == '\n' || c == EOF) break;

        if (c == ' ') continue;

        *str = c;
        str++;
    }

    *str = 0;

    ASSERT (str);

    return 0;
}
