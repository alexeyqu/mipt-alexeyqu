/** ***************************************************************************
 * @file    List_functions.cpp
 *
 * @ingroup list
******************************************************************************/

#include "Hash.h"

#define HTML_LIST_START \
{\
    if (0) (The_Log, "<td>\n");\
    if (0) (The_Log, "<Style_list>\n ");\
}

#define HTML_LIST_FINISH \
{\
    if (0) (The_Log, "</Style_list>\n ");\
    if (0) (The_Log, "</td>\n");\
}

int List_Construct (list_t* list)
{
    if (list == NULL)
    {
        return LIST_NULL;
    }

    HTEC (Elem_Construct (&list->head, ""), &list->head);
    list->length = 0;

    LIST_CHECK_ (list, list);

    return LIST_OK;
}

int List_Destruct (list_t* list)
{
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

    return LIST_OK;
}

int List_Is_OK (list_t* list)
{
  return LIST_OK;
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
    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index <= list->length; cur_index++)
    {
        Elem_Dump (cur_elem);

        cur_elem = cur_elem->next;
    }
}

int List_Add_Elem (list_t* list, elem_t* elem, unsigned int index)
{
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

    return LIST_OK;
}

int List_Delete_Elem (list_t* list, unsigned int index)
{
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

    return LIST_OK;
}

int List_Find_Key (list_t* list, const char* key, int* index)
{
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

            return LIST_OK;
        }

        cur_elem = cur_elem->next;
    }

    LIST_CHECK_ (list, list);

    KEY_CHECK_ (key);

    return -1;
}

#undef HTML_LIST_START
#undef HTML_LIST_FINISH
