/** ***************************************************************************
 * @file    Hashtable_functions.cpp
 *
 * @ingroup hash
******************************************************************************/

#include "Hash.h"

#define HTML_HT_START \
{\
    OUT (The_Log, "<tr>\n");\
    OUT (The_Log, "<Style_ht>\n ");\
}

#define HTML_HT_FINISH \
{\
    OUT (The_Log, "</Style_ht>\n ");\
    OUT (The_Log, "</tr>\n");\
}

int Hashtable_Construct (hashtable_t* hashtable, unsigned int max_hashvalue)
{
    HTML_HT_START;
    OUT (The_Log, "Constructing hashtable [%08x].. <br>\n", (int) hashtable);

    if (hashtable == NULL)
    {
        OUT (The_Log, "Pointer to hashtable [%08x] is NULL. <br>\n", (int) hashtable);
        HTML_HT_FINISH;

        return HT_NULL;
    }

    if (max_hashvalue >= MAX_HASHVALUE)
    {
        OUT (The_Log, "Max_hashvalue is too big. <br>\n");
        HTML_HT_FINISH;

        return HT_WR_HV_RANGE;
    }

    hashtable->buckets = (list_t*) calloc (max_hashvalue, sizeof (list_t));

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < max_hashvalue; cur_bucket++)
    {
        HTEC (List_Construct (&hashtable->buckets[cur_bucket]), &hashtable->buckets[cur_bucket]);
    }

    hashtable->max_hashvalue = max_hashvalue;

    HASHTABLE_CHECK_ (hashtable, hashtable);

    OUT (The_Log, "Construction of hashtable [%08x] was finished <br>\n", (int) hashtable);
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Clear (hashtable_t* hashtable)
{
    HTML_HT_START;
    OUT (The_Log, "Clearing hashtable [%08x].. <br>\n", (int) hashtable);

    HASHTABLE_CHECK_ (hashtable, hashtable);

    if (hashtable == NULL)
    {
        OUT (The_Log, "Pointer to hashtable [%08x] is NULL. <br>\n", (int) hashtable);
        HTML_HT_FINISH;

        return HT_NULL;
    }

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        HTEC (List_Construct (&hashtable->buckets[cur_bucket]), &hashtable->buckets[cur_bucket]);
    }

    HASHTABLE_CHECK_ (hashtable, hashtable);

    OUT (The_Log, "Construction of hashtable [%08x] was finished <br>\n", (int) hashtable);
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Destruct (hashtable_t* hashtable)
{
    HTML_HT_START;
    OUT (The_Log, "Destructing hashtable [%08x].. <br>\n", (int) hashtable);

    HASHTABLE_CHECK_ (hashtable, hashtable);

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        HTEC (List_Destruct (&hashtable->buckets[cur_bucket]), &hashtable->buckets[cur_bucket]);
    }

    free (hashtable->buckets);
    hashtable->max_hashvalue = 0;

    OUT (The_Log, "Destruction of hashtable [%08x] was finished <br>\n", (int) hashtable);
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Is_OK (hashtable_t* hashtable)
{
    if (hashtable == NULL) return HT_NULL;

    if (hashtable->max_hashvalue >= MAX_HASHVALUE)
    {
        OUT (The_Log, "Max_hashvalue is too big. <br>\n");
        return HT_WR_HV_RANGE;
    }
    register unsigned int i = 0;
    for (i = 0; i < hashtable->max_hashvalue; i++)
    {
        LIST_CHECK_ (&hashtable->buckets[i], &hashtable->buckets[i])
    }

    return HT_OK;
}

void Hashtable_Dump (hashtable_t* hashtable)
{
    HTML_HT_START;
    OUT (The_Log, "Dump of hashtable [%08x] started. <br>\n", (int) hashtable);

    fprintf (The_Log, "Hashtable [%08x] - %s <br>\n", (int) hashtable, (Hashtable_Is_OK (hashtable) != HT_OK)? "ok" : "ERROR");
    fprintf (The_Log, "max_hashvalue -"HTML_TAB" \t%d <br>\n", hashtable->max_hashvalue);

    register unsigned int cur_hashvalue = 0;
    for (cur_hashvalue = 0; cur_hashvalue < hashtable->max_hashvalue; cur_hashvalue++)
    {
        fprintf (The_Log, "Hashvalue #%d <br>\n", cur_hashvalue);
        List_Dump (&hashtable->buckets[cur_hashvalue]);
    }

    OUT (The_Log, "Dump of hashtable [%08x] was finished. See The_Log file for dump info. <br><br>\n", (int) hashtable);
    HTML_HT_FINISH;
}

int Hashtable_Add_Key (hashtable_t* hashtable, const char* key,
                       int (*hash) (const char* key, int* hash))
{
    HTML_HT_START;
    OUT (The_Log, "Adding key '%s' to hashtable [%08x] started. <br>\n", key, (int) hashtable);

    HASHTABLE_CHECK_ (hashtable, hashtable);

    KEY_CHECK_ (key);

    int hashvalue = 0;
    int index = -1;

    HTEC (hash (key, &hashvalue), NULL);

    hashvalue %= (hashtable->max_hashvalue);

    HTEC (List_Find_Key (&hashtable->buckets[hashvalue], key, &index), NULL);

    if (index == -1)
    {
//        OUT (The_Log, "<td>");
//        OUT (The_Log, "Adding new element to hashtable.. <br>\n");
//        OUT (The_Log, "</td>");

        elem_t* new_elem;

        HTEC (Elem_Construct (&new_elem, key), &new_elem);

        HTEC (List_Add_Elem (&hashtable->buckets[hashvalue],
              new_elem, 0), &hashtable->buckets[hashvalue]);
    }

    HASHTABLE_CHECK_ (hashtable, hashtable);

    OUT (The_Log, "Adding key to hashtable [%08x] finished. <br><br>\n", (int) hashtable);
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Fill (hashtable_t* hashtable, FILE* input,
                    int (*hash) (const char* key, int* hash))
{
    HTML_HT_START;
    OUT (The_Log, "Filling the hashtable [%08x] by words from file %d.. <br>\n", (int) hashtable, (int) input);

    HASHTABLE_CHECK_ (hashtable, hashtable);

    if (hash == NULL)
    {
        OUT (The_Log, "Pointer to hash function is NULL. <br>\n");
        HTML_HT_FINISH;

        return HT_ERR;
    }

    if (input == NULL)
    {
        OUT (The_Log, "Pointer to file is NULL. <br>\n");
        HTML_HT_FINISH;

        return HT_BAD_FILE;
    }

    fseek (input, 0, 0);

    char* word = (char*) calloc (Max_Len, sizeof (char));
    int res_input = 0;

    while (1)
    {
        res_input = fscanf (input, "%s", word);

        if (res_input == EOF)
        {
            break;
        }

        if (res_input == 0)
        {
            return HT_WR_INPUT;
        }

        HTEC (Hashtable_Add_Key (hashtable, word, hash), hashtable);
    }

    OUT (The_Log, "<u> Hashtable is ready. </u><br>\n");
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Print (hashtable_t* hashtable, FILE* output)
{
    HTML_HT_START;
    OUT (The_Log, "Printing the hashtable [%08x] into file %d.. <br>\n", (int) hashtable, (int) output);

    if (hashtable == NULL)
    {
        OUT (The_Log, "Pointer to hashtable [%08x] is NULL. <br>\n", (int) hashtable);
        HTML_HT_FINISH;

        return HT_NULL;
    }

    if (output == NULL)
    {
        OUT (The_Log, "Pointer to file is NULL. <br>\n");
        HTML_HT_FINISH;

        return HT_BAD_FILE;
    }

    fseek (output, 0, 0);

    register unsigned int cur_hashvalue = 0;

    while (cur_hashvalue < hashtable->max_hashvalue)
    {
        fprintf (output, "%d"HTML_TAB" \t%d\n", cur_hashvalue, hashtable->buckets[cur_hashvalue].length);

        cur_hashvalue++;
    }

    OUT (The_Log, "Hashtable is printed into %d file. <br>\n", (int) output);
    HTML_HT_FINISH;

    return HT_OK;
}

int Hashtable_Error_Catch (int error, const char* msg, ...)
{
    printf ("%s", msg);

    va_list args;
    va_start (args, msg);

    switch (error)
    {
        case ELEM_NULL:
            printf ("ELEM_NULL. The element wasn't initialised correctly!\n ");
            printf ("Element - [%08x]\n ", va_arg (args, int));
            getchar();
            return HT_ERR;

        case ELEM_NULL_KEY:
            printf ("ELEM_NULL_KEY. Key of element is NULL.. strange..\n ");
            printf ("Element - [%08x]\n ", va_arg (args, int));
            getchar();
            return HT_ERR;

        case ELEM_NOMEM:
            printf ("ELEM_NOMEM. Failed to allocate memory for an element!\n ");
            printf ("Element - [%08x]\n ", va_arg (args, int));

        case ELEM_WR_KEY_RANGE:
            printf ("ELEM_WR_KEY_RANGE. Wrong key size.\n ");
            printf ("Element - [%08x]\n ", va_arg (args, int));

        case LIST_NULL:
            printf ("LIST_NULL. The list wasn't initialised correctly!\n ");
            printf ("List - [%08x]\n ", va_arg (args, int));

        case LIST_INV:
            printf ("LIST_INV. Your list is somehow invalid! Call the support.\n ");
            printf ("List - [%08x]\n ", va_arg (args, int));

        case LIST_WR_NDX_RANGE:
             printf ("LIST_WR_NDX_RANGE. Wrong list index.\n ");
            printf ("List - [%08x]"HTML_TAB" \tMAX_INDEX = %d, index = %d\n ",
                    va_arg (args, int), va_arg (args, int), va_arg (args, int));

        case HT_NULL:
            printf ("HT_NULL. The hashtable wasn't initialised correctly!\n ");
            printf ("Hashtable - [%08x]\n ", va_arg (args, int));

        case HT_BAD_FILE:
            printf ("HT_BAD_FILE. The file for hashtable is invalid!\n ");
            printf ("FILE - [%08x]\n ", va_arg (args, int));

        case HT_INV:
            printf ("HT_INV. Your hashtable is full of errors! Better to call the support.\n ");
            printf ("Hashtable - [%08x]\n ", va_arg (args, int));

        case HT_WR_HV_RANGE:
            printf ("HT_WR_HV_RANGE. Wrong hashvalue range.\n ");
            printf ("Hashtable - [%08x]"HTML_TAB" \tMAX_INDEX = %d, index = %d\n ",
                    va_arg (args, int), va_arg (args, int), va_arg (args, int));

        case HT_WR_INPUT:
            printf ("HT_WR_INPUT. Something is bad inside the input file..\n ");
            printf ("FILE - [%08x]\n ", va_arg (args, int));

        default:
            return HT_OK;
    }

    getchar();
    return HT_ERR;
}

#undef HTML_HT_START
#undef HTML_HT_FINISH
