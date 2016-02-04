/** ***************************************************************************
 * @file    Elem_functions.cpp
 *
 * @ingroup elem
******************************************************************************/

#include "Hash.h"

#define HTML_ELEM_START \
{\
    OUT (The_Log, "<td>\n");\
    OUT (The_Log, "<Style_elem>\n ");\
}

#define HTML_ELEM_FINISH \
{\
    OUT (The_Log, "</Style_elem>\n ");\
    OUT (The_Log, "</td>\n");\
}

int Elem_Construct (elem_t** new_elem, const char* key)
{
    HTML_ELEM_START;
    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tConstructing new element [%08x].. <br>\n ", (int) new_elem);

    KEY_CHECK_ (key);

    *new_elem = (elem_t*) calloc (1, sizeof (elem_t));

    ELEM_CHECK_ (*new_elem, *new_elem);

    strcpy ((*new_elem)->key, key);

    (*new_elem)->amount = 0;
    (*new_elem)->prev = NULL;
    (*new_elem)->next = NULL;

    ELEM_CHECK_ (*new_elem, *new_elem);

    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tConstruction of element [%08x] was finished. <br>\n ", (int) new_elem);
    HTML_ELEM_FINISH;

    return ELEM_OK;
}

int Elem_Destruct (elem_t* elem)
{
    HTML_ELEM_START;
    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDestructing element [%08x].. <br>\n ", (int) elem);

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

    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDestruction of element [%08x] was finished. <br>\n ", (int) elem);
    HTML_ELEM_FINISH;

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
    HTML_ELEM_START;
    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDump of elem [%08x] started. <br>\n", (int) elem);

    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDump of elem [%08x] <br>\n", (int) elem);
    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tkey -"HTML_TAB" \t"HTML_TAB" \t'%s' <br>\n",             elem->key);
    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tamount -"HTML_TAB" \t%d <br>\n",              elem->amount);
    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tnext -"HTML_TAB" \t[%08x] <br>\n",      (int) elem->next);
    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tprev -"HTML_TAB" \t[%08x] <br>\n",      (int) elem->prev);
    fprintf (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDump of elem was finished. <br>\n");

    OUT (The_Log, ""HTML_TAB" \t"HTML_TAB" \tDump of elem [%08x] was finished. <br>\n", (int) elem);
    HTML_ELEM_FINISH;
}

#undef HTML_ELEM_START
#undef HTML_ELEM_FINISH
