/** ***************************************************************************
 * @file    Hash_main.cpp
 * @mainpage
 * @date    2013-12-3 13:03
 * @author  Alexey Kulikov <alexeyqu@gmail.com>, group 376, DCAM MIPT
 * @version 1.2
 *
 * Hashtable.
 *
 * @par     V1.0
 *          - Realised structures elem_t, list_t, hashtable_t ( see file @link Hash.h @endlink )
 *          - Added basic functions for working with these structures
 *          - Realised 6 different hash functions ( see file @link Hash_functions.cpp @endlink )
 *          - Added system of error catches
 *          - Added primitive print into .xls file to estimate effectiveness of hash functions
 *
 * @par     V1.1
 *          - Code refactored, The_Log prints enhanced
 *          - Doxygen comments added
 *
 * @par     V1.2
 *          - Some little corrections in code
 *          - HTML log file with simple CSS style management added
 *
 * @defgroup    elem
 * @defgroup    list
 * @defgroup    hash
 * @defgroup    misc
******************************************************************************/

#include "Hash.h"

FILE* The_Log = NULL;

int main()
{
    printf ("Hello!\n");
    printf ("I'm a program that constructs a hashtable and "
            "fills it with words from text.\n");
    printf ("There are 6 different hash functions (see documentation).\n");
    printf ("After the hashtable is filled up it is printed into suitable .xls file.\n");
    printf ("The program prints also timing of each function.\n");
    printf ("So you can compare them by different parametres \n"
            "(e.g. time elapsed, usefulness etc.)\n\n");
    printf ("Author: AQ\n");
    printf ("Compiled from %s file\n"
            "on %s %s\n\n", __FILE__, __DATE__, __TIME__);

    clock_t total_start = 0, total_finish = 0;

    total_start = clock();

        The_Log = fopen ("The_Log.html",  "w");
    FILE*  text = fopen ("text.txt",  "r");
    FILE* input = fopen ("input.txt", "w+");
    FILE* output = NULL;

    ASSERT (The_Log); ASSERT (text); ASSERT (input);

    {
        fprintf (The_Log, "<html>\n");

        fprintf (The_Log, "<head>\n");

        fprintf (The_Log, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> \n");
        fprintf (The_Log, "<title>Hashtable log</title> \n");
        fprintf (The_Log, "<style type=\"text/css\"> \n");

        fprintf (The_Log, "Style_elem\n");
        fprintf (The_Log, "{ \n");
        fprintf (The_Log, "\tfont-size: 75%%; \n");
        fprintf (The_Log, "\tfont-family: Arial; \n");
        fprintf (The_Log, "color: #505050; \n");
        fprintf (The_Log, "} \n");

        fprintf (The_Log, "Style_list\n");
        fprintf (The_Log, "{ \n");
        fprintf (The_Log, "\tfont-size: 85%%; \n");
        fprintf (The_Log, "\tfont-family: Arial; \n");
        fprintf (The_Log, "color: #0000FF; \n");
        fprintf (The_Log, "} \n");

        fprintf (The_Log, "Style_ht\n");
        fprintf (The_Log, "{ \n");
        fprintf (The_Log, "\tfont-size: 100%%; \n");
        fprintf (The_Log, "\tfont-family: Arial; \n");
        fprintf (The_Log, "color: #00BB00; \n");
        fprintf (The_Log, "} \n");

        fprintf (The_Log, "Style_ERROR\n");
        fprintf (The_Log, "{ \n");
        fprintf (The_Log, "\tfont-size: 200%%; \n");
        fprintf (The_Log, "\tfont-family: Arial; \n");
        fprintf (The_Log, "color: #FF0000; \n");
        fprintf (The_Log, "} \n");

        fprintf (The_Log, "Style_basic\n");
        fprintf (The_Log, "{ \n");
        fprintf (The_Log, "\tfont-size: 100%%; \n");
        fprintf (The_Log, "\tfont-family: Comic Sans MS; \n");
        fprintf (The_Log, "color: #000000; \n");
        fprintf (The_Log, "} \n");

        fprintf (The_Log, "</style>\n");

        fprintf (The_Log, "</head>\n");
    }

    hashtable_t hashtable = {};

    printf ("Please enter size of hashtable:\n->");

    unsigned int max_hashvalue = 0;
    int res_input = scanf ("%d", &max_hashvalue);

    ASSERT (res_input);
    ASSERT (max_hashvalue < MAX_HASHVALUE);

    fprintf (The_Log, "<body>\n ");
    fprintf (The_Log, "<Style_basic>\n ");

    fprintf (The_Log, "<b> Hashtable project log. <br>\n ");
    fprintf (The_Log, "Author: AQ </b> <br>\n ");
    fprintf (The_Log, "<i> Compiled from %s file "
            "on %s %s </i> <br><br>\n", __FILE__, __DATE__, __TIME__);

    HTEC (Adapt_Input (text, input), NULL);

    fprintf (The_Log, "Constructing the hashtable: <br>\n ");

    fprintf (The_Log, "</Style_basic>\n ");

    HTEC (Hashtable_Construct (&hashtable, max_hashvalue), &hashtable);

    clock_t t_start = 0, t_finish = 0; //!< For timing hash functions

    #define TRY_HASH_( num )\
    {\
        printf ("Processing #%d hash function..\n ", num);\
        \
        fprintf (The_Log, "<br><b> Executing the hash function #%d.. </b><br><br>\n ", num);\
        \
            output = fopen ("output-"#num".csv", "w");\
            ASSERT (output);\
        t_start = clock();\
        fprintf (The_Log, "<table>\n");\
            HTEC (Hashtable_Fill (&hashtable, input, hash_##num), &hashtable);\
        fprintf (The_Log, "</table> <br>\n");\
        t_finish = clock();\
        \
        printf ("Time elapsed: %lg\n\n", (double) ((double) (t_finish) - (double) t_start)/CLOCKS_PER_SEC);\
        fprintf (The_Log, "<b>Time elapsed: %lg </b><br><br>\n", (double) ((double) (t_finish) - (double) t_start)/CLOCKS_PER_SEC);\
        fprintf (The_Log, "<br><b> Printing the hashtable.. </b><br>\n ");\
        \
        HTEC (Hashtable_Print (&hashtable, output), &hashtable);\
        fprintf (The_Log, "<br><b> Clearing the hashtable.. </b><br>\n ");\
        HTEC (Hashtable_Clear (&hashtable), &hashtable);\
        fprintf (The_Log, "<br><b> Hashtable is clean now. </b><br>\n ");\
        fprintf (The_Log, "<br>\n ");\
            fclose (output);\
        /*system ("libreoffice output-"#num".xls");*/\
    }

    TRY_HASH_ (1);
    TRY_HASH_ (2);
    TRY_HASH_ (3);
    TRY_HASH_ (4);
    TRY_HASH_ (5);
    TRY_HASH_ (6);

    #undef TRY_HASH_

    fprintf (The_Log, "<Style_basic>\n ");

    fprintf (The_Log, "<b> Destructing the hashtable: </b><br>\n ");

    fprintf (The_Log, "<Style_basic>\n ");

    HTEC (Hashtable_Destruct (&hashtable), &hashtable);

    total_finish = clock();

    OUT (The_Log, "<b><br> Total time elapsed: %lg<br> \n\n", (double) ((double) (total_finish) - (double) total_start)/CLOCKS_PER_SEC);
    OUT (The_Log, "<br> (c) AQ, 2013</b><br> \n\n");

    fprintf (The_Log, "</body>\n ");
    fprintf (The_Log, "</html>\n ");

    fclose (The_Log);
    fclose (input);
    fclose (text);

    return 0;
}
