/** ***************************************************************************
 * @file    Elem_functions.cpp
 *
 * @ingroup elem
******************************************************************************/

#include "Hash.h"

#define HTML_ELEM_START \
{\
    if (0) (The_Log, "<td>\n");\
    if (0) (The_Log, "<Style_elem>\n ");\
}

#define HTML_ELEM_FINISH \
{\
    if (0) (The_Log, "</Style_elem>\n ");\
    if (0) (The_Log, "</td>\n");\
}

int Elem_Construct (elem_t** new_elem, const char* key)
{
//    //HTML_ELEM_START;
//    if (0) (The_Log, ""//HTML_TAB" \t"//HTML_TAB" \tConstructing new element [%08x].. <br>\n ", (int) new_elem);

    KEY_CHECK_ (key);

    *new_elem = (elem_t*) calloc (1, sizeof (elem_t));

    ELEM_CHECK_ (*new_elem, *new_elem);

    strcpy ((*new_elem)->key, key);

    (*new_elem)->amount = 0;
    (*new_elem)->prev = NULL;
    (*new_elem)->next = NULL;

    ELEM_CHECK_ (*new_elem, *new_elem);

//    if (0) (The_Log, ""//HTML_TAB" \t"//HTML_TAB" \tConstruction of element [%08x] was finished. <br>\n ", (int) new_elem);
//    //HTML_ELEM_FINISH;

    return ELEM_OK;
}

int Elem_Destruct (elem_t* elem)
{
    ELEM_CHECK_ (elem, elem);

    elem->prev = NULL;
    elem->next = NULL;
    elem->amount = Poison;

    register unsigned int i = 0;
    for (i = 0; i < Max_Len; i++)
    {
        elem->key[i] = 0;
    }

    free (elem);

    return ELEM_OK;
}

int Elem_Is_OK (elem_t* elem)
{
    if (elem == NULL) return ELEM_NULL;

    if (elem->key == NULL) return ELEM_NULL_KEY;

    return ELEM_OK;
}

void Elem_Dump (elem_t* elem)
{

}

#undef HTML_ELEM_START
#undef HTML_ELEM_FINISH
