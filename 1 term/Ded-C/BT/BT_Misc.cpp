#include "BT.h"

void bin_printf (FILE* outstream, char c)
{
    unsigned register int i = 0;

    while (i < sizeof (char)*8)
    {
        if (c & 1) fprintf (outstream, "1");
        else       fprintf (outstream, "0");

        c  >>= 1;

        i++;
    }

    fprintf (outstream, "\n");

    return;
}
