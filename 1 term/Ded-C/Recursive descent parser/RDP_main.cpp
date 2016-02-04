/** ***************************************************************************
 * @file    RDP_main.cpp
 * @mainpage
 * @date    2014-01-28 14:14
 * @author  Alexey Kulikov <alexeyqu@gmail.com>, group 376, DCAM MIPT
 * @version 1.01
 *
 * Recursive descent parser.
 *
 * @par     V1.00
 *          Realised grammars G0..G4.
 *          The G4 one can read and calculate any simple mathematical expression
 * @par     V1.01
 *          Added doxygen comments
******************************************************************************/

#include "RDP.h"

char* s = NULL;

int main()
{
    time_t t_now = time (0);

    struct tm* now = localtime (&t_now);

    printf ("Hi!\n");
    printf ("I'm just a smart calculator.\n"
            "I use the method of recursive descent parsing.\n\n");
    printf ("Author: AQ\n");
    printf ("Compiled from %s file\n"
            "on %s %s\n", __FILE__, __DATE__, __TIME__);
    printf ("Executed on %d-%d-%d %d:%d:%d\n", now->tm_mon + 1, now->tm_mday, now->tm_year + 1900,
            now->tm_hour, now->tm_min, now->tm_sec);
    printf ("--------------------\n\n");

    char* str = (char*) calloc (Max_Len, sizeof (char));
    FILE* instream =  fopen ("input.txt",  "r");
    FILE* outstream = fopen ("output.txt", "w");
    FILE* The_Log =   fopen ("log.txt",    "w");

    ASSERT (instream); ASSERT (outstream); ASSERT (The_Log);

    Get_String (instream, str);

    printf ("%lg\n", Get_G4 (str));

    return 0;
}
