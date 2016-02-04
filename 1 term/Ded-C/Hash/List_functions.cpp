/** ***************************************************************************
 * @file    List_functions.cpp
 *
 * @ingroup list
******************************************************************************/

#include "Hash.h"

#define HTML_LIST_START \
{\
    OUT (The_Log, "<td>\n");\
    OUT (The_Log, "<Style_list>\n ");\
}

#define HTML_LIST_FINISH \
{\
    OUT (The_Log, "</Style_list>\n ");\
    OUT (The_Log, "</td>\n");\
}

int List_Construct (list_t* list)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tConstruction of new list [%08x].. <br>\n", (int) list);

    if (list == NULL)
    {
        OUT (The_Log, ""HTML_TAB" \tPointer to list [%08x] is NULL. <br>\n", (int) list);
        HTML_LIST_FINISH;

        return LIST_NULL;
    }

    HTEC (Elem_Construct (&list->head, ""), &list->head);
    list->length = 0;

    LIST_CHECK_ (list, list);

    OUT (The_Log, ""HTML_TAB" \tConstruction of list [%08x] was finished. <br><br>\n", (int) list);
    HTML_LIST_FINISH;

    return LIST_OK;
}

int List_Destruct (list_t* list)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tDestructing list [%08x].. <br>\n", (int) list);

    LIST_CHECK_ (list, list);

    elem_t* cur_elem = list->head;
    elem_t* next_elem = NULL;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < list->length; cur_index++)
    {
        next_elem = cur_elem->next;

        HTEC (Elem_Destruct (cur_elem), cur_elem);

        cur_elem = next_elem;
    }

    list->length = Poison;

    OUT (The_Log, ""HTML_TAB" \tDestruction of list [%08x] was finished. <br><br>\n", (int) list);
    HTML_LIST_FINISH;

    return LIST_OK;
}

int List_Is_OK (list_t* list)
{
    if (list == NULL) return LIST_NULL;
    if (list->length < 0) return LIST_WR_NDX_RANGE;

    elem_t* cur_elem = list->head;

    unsigned int i = 0;
    for (i = 0; i < list->length; i++)
    {
        if (cur_elem->next == NULL ||
            cur_elem->next->prev != cur_elem) return LIST_INV;

        cur_elem = cur_elem->next;
    }

    return LIST_OK;
}

void List_Dump (list_t* list)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tDump of list [%08x] started. <br>\n", (int) list);

    fprintf (The_Log, ""HTML_TAB" \tList [%08x] - %s <br>\n", (int) list, (List_Is_OK (list) == LIST_OK)? "ok" : "ERROR");
    fprintf (The_Log, ""HTML_TAB" \tlength -"HTML_TAB" \t%d <br>\n", list->length);

    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index <= list->length; cur_index++)
    {
        fprintf (The_Log, ""HTML_TAB" \tElement #%d <br>\n", cur_index);
        Elem_Dump (cur_elem);

        cur_elem = cur_elem->next;
    }

    OUT (The_Log, ""HTML_TAB" \tDump of list [%08x] was finished. <br>\n", (int) list);
    HTML_LIST_FINISH;
}

int List_Add_Elem (list_t* list, elem_t* elem, unsigned int index)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tAdding element [%08x] to list [%08x] started. <br>\n", (int) elem, (int) list);
    OUT (The_Log, ""HTML_TAB" \tIndex = %d <br>\n", index);

    LIST_CHECK_ (list, list);

    ELEM_CHECK_ (elem, elem);

    INDEX_CHECK_ (index);

    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < index; cur_index++)
    {
        cur_elem = cur_elem->next;
    }

    elem->next = cur_elem->next;
    elem->prev = cur_elem;
    cur_elem->next = elem;
    if (elem->next != NULL) elem->next->prev = elem;

    list->length++;

    LIST_CHECK_ (list, list);

    ELEM_CHECK_ (elem, elem);

    INDEX_CHECK_ (index);

    OUT (The_Log, ""HTML_TAB" \tAdding element to list [%08x] finished. <br>\n", (int) list);
    HTML_LIST_FINISH;

    return LIST_OK;
}

int List_Delete_Elem (list_t* list, unsigned int index)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tDeleting element #%d to list [%08x] started. <br>\n", index, (int) list);

    LIST_CHECK_ (list, list);

    INDEX_CHECK_ (index);

    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < index; cur_index++)
    {
        cur_elem = cur_elem->next;
    }

    cur_elem->prev->next = cur_elem->next;
    cur_elem->next->prev = cur_elem->prev;

    list->length--;

    LIST_CHECK_ (list, list);

    INDEX_CHECK_ (index);

    OUT (The_Log, ""HTML_TAB" \tDeleting element from list [%08x] finished. <br>\n", (int) list);
    HTML_LIST_FINISH;

    return LIST_OK;
}

int List_Find_Key (list_t* list, const char* key, int* index)
{
    HTML_LIST_START;
    OUT (The_Log, ""HTML_TAB" \tFinding the key '%s' inside the list [%08x].. <br>\n", key, (int) list);

    *index = -1;

    LIST_CHECK_ (list, list);

    KEY_CHECK_ (key);

    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < list->length; cur_index++)
    {
        if (strcmp (cur_elem->key, key) == 0)
        {
            *index = cur_index;

            OUT (The_Log, ""HTML_TAB" \tKey found, index = %d <br>\n", *index);
            HTML_LIST_FINISH;

            return LIST_OK;
        }

        cur_elem = cur_elem->next;
    }

    LIST_CHECK_ (list, list);

    KEY_CHECK_ (key);

    OUT (The_Log, ""HTML_TAB" \tKey wasn't found <br>\n");
    HTML_LIST_FINISH;

    return -1;
}

/*int List_Print (list_t* list, FILE* output)
{
    OUT (The_Log, "Printing the list [%08x] into file %d..\n ", (int) list, (int) output);

    if (hashtable == NULL)
    {
        OUT (The_Log, "Pointer to list [%08x] is NULL.\n ", (int) list);
        return HT_NULL;
    }

    if (output == NULL)
    {
        OUT (The_Log, "Pointer to file is NULL.\n ");
        return HT_BAD_FILE;
    }

    register unsigned int cur_index = 0;

    while (cur_index < list->length)
    {
        fprintf (output, "%s %d\n ", etc)
    }

    OUT (The_Log, "List [%08x] is printed into %d file.\n ", (int) list, (int) output);

    return HT_OK;
}
*/

#undef HTML_LIST_START
#undef HTML_LIST_FINISH
