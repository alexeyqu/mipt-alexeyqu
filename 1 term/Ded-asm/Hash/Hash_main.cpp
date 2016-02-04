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
 *          - //HTML log file with simple CSS style management added
 *
 * @defgroup    elem
 * @defgroup    list
 * @defgroup    hash
 * @defgroup    misc
******************************************************************************/

#include "Hash.h"

int main()
{
    FILE*  text = fopen ("text.txt",  "r");
    FILE* input = fopen ("input.txt", "w+");

    hashtable_t hashtable = {};

    unsigned int max_hashvalue = 100;

    HTEC (Adapt_Input (text, input), NULL);

    HTEC (Hashtable_Construct (&hashtable, max_hashvalue), &hashtable);

    char* input_data[DATA_SIZE] = {};

    Data_Fill (input_data, input);

    fclose (input);
    fclose (text);

    #define TRY_HASH_( num )\
    {\
        HTEC (Hashtable_Fill (&hashtable, input_data, hash_##num), &hashtable);\
        HTEC (Hashtable_Clear (&hashtable), &hashtable);\
    }

    for (int i = 0; i < 1000; i++)
    {
        TRY_HASH_ (6);
    }

    #undef TRY_HASH_

    HTEC (Hashtable_Destruct (&hashtable), &hashtable);

    return 0;
}
