/** ***************************************************************************
 * @file    Hashtable_functions.cpp
 *
 * @ingroup hash
******************************************************************************/

#include "Hash.h"

#define HTML_HT_START \
{\
    if (0) (The_Log, "<tr>\n");\
    if (0) (The_Log, "<Style_ht>\n ");\
}

#define HTML_HT_FINISH \
{\
    if (0) (The_Log, "</Style_ht>\n ");\
    if (0) (The_Log, "</tr>\n");\
}

int Hashtable_Construct (hashtable_t* hashtable, unsigned int max_hashvalue)
{
    if (hashtable == NULL)
    {
        return HT_NULL;
    }

    if (max_hashvalue >= MAX_HASHVALUE)
    {
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

    return HT_OK;
}

int Hashtable_Clear (hashtable_t* hashtable)
{
    HASHTABLE_CHECK_ (hashtable, hashtable);

    if (hashtable == NULL)
    {
        return HT_NULL;
    }

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        HTEC (List_Construct (&hashtable->buckets[cur_bucket]), &hashtable->buckets[cur_bucket]);
    }

    HASHTABLE_CHECK_ (hashtable, hashtable);

    return HT_OK;
}

int Hashtable_Destruct (hashtable_t* hashtable)
{
    HASHTABLE_CHECK_ (hashtable, hashtable);

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        HTEC (List_Destruct (&hashtable->buckets[cur_bucket]), &hashtable->buckets[cur_bucket]);
    }

    free (hashtable->buckets);
    hashtable->max_hashvalue = 0;

    return HT_OK;
}

int Hashtable_Is_OK (hashtable_t* hashtable)
{
    return HT_OK;

    if (hashtable == NULL) return HT_NULL;

    if (hashtable->max_hashvalue >= MAX_HASHVALUE)
    {
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
    register unsigned int cur_hashvalue = 0;
    for (cur_hashvalue = 0; cur_hashvalue < hashtable->max_hashvalue; cur_hashvalue++)
    {
        List_Dump (&hashtable->buckets[cur_hashvalue]);
    }
}

int Hashtable_Add_Key (hashtable_t* hashtable, const char* key,
                       int (*hash) (const char* key, int* hash))
{
    HASHTABLE_CHECK_ (hashtable, hashtable);

    int hashvalue = 0;
    int index = -1;

    HTEC (hash (key, &hashvalue), NULL);

    hashvalue %= (hashtable->max_hashvalue);

    HTEC (List_Find_Key (&hashtable->buckets[hashvalue], key, &index), NULL);

    if (index == -1)
    {
        elem_t* new_elem;

        HTEC (Elem_Construct (&new_elem, key), &new_elem);

        HTEC (List_Add_Elem (&hashtable->buckets[hashvalue],
              new_elem, 0), &hashtable->buckets[hashvalue]);
    }

    HASHTABLE_CHECK_ (hashtable, hashtable);

    return HT_OK;
}

int Hashtable_Fill (hashtable_t* hashtable, char* input_data[],
                    int (*hash) (const char* key, int* hash))
{
    HASHTABLE_CHECK_ (hashtable, hashtable);

    if (hash == NULL)
    {
        return HT_ERR;
    }

    int i = 0;

    while (i < 100)
    {
        HTEC (Hashtable_Add_Key (hashtable, input_data[i], hash), hashtable);

        i++;
    }

    return HT_OK;
}

int Hashtable_Print (hashtable_t* hashtable, FILE* output)
{
    if (hashtable == NULL)
    {
        return HT_NULL;
    }

    if (output == NULL)
    {
        return HT_BAD_FILE;
    }

    fseek (output, 0, 0);

    register unsigned int cur_hashvalue = 0;

    while (cur_hashvalue < hashtable->max_hashvalue)
    {
        cur_hashvalue++;
    }

    return HT_OK;
}

void Data_Fill (char* input_data[], FILE* input)
{
    fseek (input, 0, 0);

    char* word = (char*) calloc (Max_Len, sizeof (char));
    int res_input = 0;
    int i = 0;

    while (1)
    {
        input_data[i] = (char*) calloc (Max_Len, sizeof (char));
        res_input = fscanf (input, "%s", word);

        if (res_input == EOF || i >= DATA_SIZE)
        {
            break;
        }

        strcpy (input_data[i], word);
        i++;
    }
}

int Hashtable_Error_Catch (int error, const char* msg, ...)
{
    return HT_OK;
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

