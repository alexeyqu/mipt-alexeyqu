
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// hashtable.c
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "hashtable.h"

#define DEBUG 1 // ?

#ifndef DEBUG
    #error Define DEBUG as 0 or 1!
#endif

#define HTEC( func ) if (hashtable_error_catch (func) == HT_ERR) RETURN (!"Here it was:");

#define KEY_CHECK_( key )\
    if (key == NULL)\
    {\
        return hashtable_error_catch (ELEM_NULL_KEY);\
    }\
    if (strlen (key) >= MAX_KEY_LEN)\
    {\
        return hashtable_error_catch (ELEM_WR_RANGE);\
    }

#define BLOB_CHECK_( blob )\
    if (blob == NULL)\
    {\
        return hashtable_error_catch (ELEM_NULL_BLOB);\
    }

#define INDEX_CHECK_( index )\
    if (index > list->length)\
    {\
        return hashtable_error_catch (LIST_WR_NDX_RANGE);\
    }

#define LIST_CHECK_( list ) HTEC (list_is_ok (list));

#define ELEM_CHECK_( elem ) HTEC (elem_is_ok (elem));

#define HASHTABLE_CHECK_( hashtable ) HTEC (hashtable_is_ok (hashtable));

#define ITERATOR_CHECK_( iterator ) HTEC (iterator_is_ok (iterator));

#define HASH_CHECK_( hash )\
if (hash == NULL)\
{\
	return HT_NULL;\
}

#if (DEBUG == 1)
    #define OUT fprintf
    #define RETURN( cond )\
        if (!(cond))\
        {\
            fprintf (stdout, "Assserion <%s> failed!\n ", #cond);\
            fprintf (stdout, "Info: file %s, line %d\n ", __FILE__, __LINE__);\
            return HT_ERR;\
        }
#else
    #define OUT if (0)
    #define RETURN( cond ) return HT_ERR
#endif

const unsigned int POISON = 0xDEAD;

#define MAX_KEY_LEN 50
#define MAX_BLOB_LEN 1000
#define MAX_HASHVALUE 10000

/*typedef struct dpiece_t
{
	char key[MAX_KEY_LEN];
	char* blob;
	int blob_size;
} dpiece_t;*/

extern int UNLUCK;

typedef struct elem_t elem_t;

struct elem_t
{
	char key[MAX_KEY_LEN];
	char blob[MAX_BLOB_LEN];
	int blob_size;

	elem_t* next;
	elem_t* prev;
};

typedef struct list_t
{
	unsigned int length;
	elem_t* head;
} list_t;

struct hashtable_t
{
	list_t* buckets;
	unsigned int max_hashvalue;
};

struct ht_iterator_t
{
	hashtable_t hashtable;
	unsigned cur_bucket;
	elem_t* cur_elem;
};

enum ERR_ELEM
{
    ELEM_OK             = 1000,
    ELEM_NULL           = 1001,
    ELEM_NULL_KEY       = 1002,
    ELEM_NULL_BLOB      = 1003,
    ELEM_WR_RANGE       = 1004
};

enum ERR_LIST
{
    LIST_OK           = 2000,
    LIST_NULL         = 2001,
    LIST_WR_NDX_RANGE = 2002
};

enum ERR_HASHTABLE
{
    HT_ERR          = -1,
    HT_OK           = 3000,
    HT_NULL         = 3001,
    HT_WR_HV_RANGE  = 3002,
    HT_WR_INPUT     = 3004
};

enum ERR_ITERATOR
{
	IT_OK 		= 4000,
	IT_NULL 	= 4001
};

static int hashtable_error_catch (int error);
static void* my_calloc (size_t num, size_t size);

//==============================================================================
//------------------------------------------------------------------------------
//------------------------------------HASH--------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

static int ROL (int arg, int shift)
{
    return (arg << shift) | (arg >> (sizeof(arg)*sizeof (int) - shift));;
}

static int hash (const char* key, int* hash)
{
    int sum = 0;

    unsigned int length = strlen (key);
    register unsigned int i = 0;
    for (i = 0; i < length; i++)
    {
        sum += sum ^ ROL ((int) key[i], 1);
        sum = sum ^ key[i];
    }

    sum /= strlen (key);

    *hash = sum;

    *hash = key[0]; //--that's another hash function (easier to cause collisions)

    return HT_OK;
}

//==============================================================================
//------------------------------------------------------------------------------
//------------------------------------ELEM--------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

static int elem_is_ok (elem_t* elem) 
{
	if (elem == NULL)
	{
		return ELEM_NULL;
	}

	return HT_OK;
}

static int elem_construct (elem_t** new_elem, char key[], char blob[], int blob_size) 
{
	*new_elem = (elem_t*) my_calloc (1, sizeof (elem_t));

    if (hashtable_error_catch (elem_is_ok (*new_elem)) == HT_ERR) 
    {
        free (*new_elem);
        return HT_ERR;
    }

	strcpy ((*new_elem)->key, key);
	memmove ((*new_elem)->blob, blob, blob_size);

	(*new_elem)->blob_size = blob_size;
	(*new_elem)->prev = NULL;
	(*new_elem)->next = NULL;

	return HT_OK;
}

static int elem_destruct (elem_t* elem)
{
	register unsigned int i = 0;
	for (i = 0; i < MAX_KEY_LEN; i++)
	{
		elem->key[i] = POISON;
	}

	for (i = 0; i < elem->blob_size; i++)
	{
		elem->blob[i] = POISON;
	}

	elem->prev = NULL;
	elem->next = NULL;
	elem->blob_size = 0;

	free (elem);

	return HT_OK;
}

//==============================================================================
//------------------------------------------------------------------------------
//------------------------------------LIST--------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

static int list_is_ok (list_t* list) 
{
    elem_t* cur_elem = list->head;

    unsigned int i = 0;
    for (i = 0; i < list->length; i++)
    {
        if (cur_elem->next == NULL ||
                (cur_elem->next == NULL &&
                i > 0) ||
            cur_elem->next->prev != cur_elem) 
        {
        	return LIST_NULL;
        }

        if (hashtable_error_catch (elem_is_ok (cur_elem)) != HT_OK)
        {
            return HT_ERR;
        }

        cur_elem = cur_elem->next;
    }

    return LIST_OK;
}

static int list_construct (list_t* list) 
{
	if (elem_construct (&list->head, "\0", "\0", 0) != HT_OK)
    {
        return HT_ERR;
    }

	list->length = 0;

    return LIST_OK;
}

static int list_destruct (list_t* list)
{
    elem_t* cur_elem = list->head;
    elem_t* next_elem = NULL;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < list->length; cur_index++)
    {
        next_elem = cur_elem->next;

        elem_destruct (cur_elem);

        cur_elem = next_elem;
    }

    list->length = POISON;

    return LIST_OK;
}

static int list_add_elem (list_t* list, elem_t* elem, unsigned int index)
{
    elem_t* cur_elem = list->head;

    elem->next = cur_elem->next;
    elem->prev = cur_elem;
    cur_elem->next = elem;
    if (elem->next != NULL) 
    {
    	elem->next->prev = elem;
    }

    list->length++;

    return LIST_OK;
}

static int list_delete_elem (list_t* list, unsigned int index)
{
    elem_t* cur_elem = list->head;

    register unsigned int cur_index = 0;
    for (cur_index = 0; cur_index < index; cur_index++)
    {
        cur_elem = cur_elem->next;
    }

    cur_elem->prev->next = cur_elem->next;
    if (cur_elem->next != NULL)
    {
    	cur_elem->next->prev = cur_elem->prev;
    }

    elem_destruct (cur_elem);

    list->length--;

    return LIST_OK;
}

static int list_find_key (list_t* list, const char* key, int* index)
{
    *index = -1;

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

    *index = -1;

    return HT_OK;
}

//==============================================================================
//------------------------------------------------------------------------------
//-------------------------------------HT---------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

static int hashtable_is_ok (hashtable_t hashtable) 
{
    if (hashtable == NULL)
    {
    	return HT_NULL;
    }

    register unsigned int i = 0;
    for (i = 0; i < hashtable->max_hashvalue; i++)
    {
        if (list_is_ok (&hashtable->buckets[i]) != LIST_OK)
        {
            return HT_ERR;
        }
    }

    return HT_OK;
}

hashtable_t hashtable_construct (unsigned int max_hashvalue, int* error_value) 
{
	if (error_value == NULL)
	{
		return hashtable_error_catch (HT_NULL), NULL;
	}

	if (max_hashvalue >= MAX_HASHVALUE)
	{
		*error_value = HT_WR_HV_RANGE;
		return hashtable_error_catch (HT_WR_HV_RANGE), NULL;
	}

	hashtable_t hashtable = (hashtable_t) my_calloc (1, sizeof (hashtable_t));
	if (hashtable == NULL)
	{
         free (hashtable); 
		*error_value = HT_NULL;
		return hashtable_error_catch (HT_NULL), NULL;
	}

	hashtable->buckets = (list_t*) my_calloc (max_hashvalue + 1, sizeof (list_t));
	if (hashtable->buckets == NULL)
	{
        free (hashtable->buckets); 
        free (hashtable); 
		*error_value = HT_NULL;
		return hashtable_error_catch (HT_NULL), NULL;
	}

	register unsigned int cur_bucket = 0, res_list_construct = 0;
	for (cur_bucket = 0; cur_bucket <= max_hashvalue; cur_bucket++)
	{
		res_list_construct = list_construct (&hashtable->buckets[cur_bucket]);

		if (res_list_construct != LIST_OK)
		{
            register unsigned int j = 0;
            for (j = 0; j <= cur_bucket; j++)
            {
                list_destruct (&hashtable->buckets[j]);
            }

            free (hashtable->buckets); 
            free (hashtable); 
			*error_value = res_list_construct;
			return hashtable_error_catch (res_list_construct), NULL;
		}
	}

	hashtable->max_hashvalue = max_hashvalue + 1;

	*error_value = 0;

	return hashtable;
}

int hashtable_clear (hashtable_t hashtable) 
{
    if (hashtable_error_catch (hashtable_is_ok (hashtable)) != HT_OK)
    {
        return HT_ERR;
    }

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        if (hashtable_error_catch (list_construct (&hashtable->buckets[cur_bucket])) != HT_OK)
        {
            return HT_ERR;
        }
    }

    return HT_OK;
}

int hashtable_destruct (hashtable_t hashtable) 
{
    if (hashtable_error_catch (hashtable_is_ok (hashtable)) != HT_OK)
    {
        return HT_ERR;
    }

    register unsigned int cur_bucket = 0;
    for (cur_bucket = 0; cur_bucket < hashtable->max_hashvalue; cur_bucket++)
    {
        list_destruct (&hashtable->buckets[cur_bucket]);
    }

    free (hashtable->buckets); 
    hashtable->max_hashvalue = 0;

    return HT_OK;
}

int hashtable_add (hashtable_t hashtable,  char key[], char blob[], int blob_size) 
{    
    if (hashtable_error_catch (hashtable_is_ok (hashtable)) != HT_OK)
    {
        return HT_ERR;
    }

    if (key == NULL)
    {
        return hashtable_error_catch (ELEM_NULL_KEY);
    }

    if (strlen (key) >= MAX_KEY_LEN)
    {
        return hashtable_error_catch (ELEM_WR_RANGE);
    }

    if (blob == NULL)
    {
        return hashtable_error_catch (ELEM_NULL_BLOB);
    }

    if (blob_size < 1 ||
        blob_size > MAX_BLOB_LEN)
    {
        return HT_ERR;
    }

    int hashvalue = 0;
    int index = -1;

    hash (key, &hashvalue);

    hashvalue %= (hashtable->max_hashvalue);

    if (hashtable_error_catch (list_find_key (&hashtable->buckets[hashvalue], key, &index)) != HT_OK)
    {
        return HT_ERR;
    }

    if (index == -1)
    {
        elem_t* new_elem;

        if (hashtable_error_catch (elem_construct (&new_elem, key, blob, blob_size)) != HT_OK)
        {
            return HT_ERR;
        }

        if (hashtable_error_catch (list_add_elem (&hashtable->buckets[hashvalue], new_elem, 0)) != HT_OK)
         {
            return HT_ERR;
        }
    }

    return HT_OK;
}

int hashtable_del (hashtable_t hashtable, char key[]) 
{
    if (hashtable_error_catch (hashtable_is_ok (hashtable)) != HT_OK)
    {
        return HT_ERR;
    }

    if (key == NULL)
    {
        return hashtable_error_catch (ELEM_NULL_KEY);
    }

    if (strlen (key) >= MAX_KEY_LEN)
    {
        return hashtable_error_catch (ELEM_WR_RANGE);
    }

    int hashvalue = 0;
    int index = -1;

    hash (key, &hashvalue);

    hashvalue %= (hashtable->max_hashvalue);

    if (hashtable_error_catch (list_find_key (&hashtable->buckets[hashvalue], key, &index)) != HT_OK)
    {
        return HT_ERR;
    }

    if (index == -1)
    {
    	return hashtable_error_catch (HT_WR_INPUT);
    }

    if (hashtable_error_catch (list_delete_elem (&hashtable->buckets[hashvalue], index))!= HT_OK)
    {
        return HT_ERR;
    }

    return HT_OK;
}

static int hashtable_error_catch (int error)
{
    switch (error)
    {
        case ELEM_NULL:
            printf ("ELEM_NULL. The element wasn't initialised correctly!\n ");
            return HT_ERR;

        case ELEM_NULL_KEY:
            printf ("ELEM_NULL_KEY. Key of element is NULL.. strange..\n ");
            return HT_ERR;

        case ELEM_WR_RANGE:
            printf ("ELEM_WR_RANGE. Wrong key size.\n ");
            return HT_ERR;

        case LIST_NULL:
            printf ("LIST_NULL. The list wasn't initialised correctly!\n ");
            return HT_ERR;

        case HT_NULL:
            printf ("HT_NULL. The hashtable wasn't initialised correctly!\n ");
            return HT_ERR;

        case HT_WR_HV_RANGE:
            printf ("HT_WR_HV_RANGE. Wrong hashvalue range.\n ");
            return HT_ERR;

        case HT_WR_INPUT:
            printf ("HT_WR_INPUT. Something is bad inside the input file..\n ");
            return HT_ERR;

        case IT_NULL:
        	printf ("IT_NULL. Something is wrong with the iterator.\n ");
        	return HT_ERR;

        case HT_ERR:
            printf ("HT_ERR. \n");
            return HT_ERR;

        default:
            return HT_OK;
    }

    return HT_ERR;
}

//==============================================================================
//------------------------------------------------------------------------------
//----------------------------------ITERATOR------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

int iterator_is_ok (ht_iterator_t iterator)
{
	if (iterator == NULL ||
		iterator->cur_bucket > iterator->hashtable->max_hashvalue ||
		iterator->cur_bucket < 0)
	{
		return IT_NULL;
	}

	return IT_OK;
}

ht_iterator_t iterator_construct (hashtable_t hashtable, int* error_value)
{
	if (hashtable_is_ok (hashtable) != HT_OK)
	{
		*error_value = HT_ERR;
		return NULL;
	}

	if (error_value == NULL)
	{
		return NULL;
	}

	ht_iterator_t iterator = (ht_iterator_t) my_calloc (1, sizeof (ht_iterator_t));
	if (iterator == NULL)
	{
        free (iterator); 
		*error_value = hashtable_error_catch (IT_NULL);
		return NULL;
	}

	iterator->hashtable = hashtable;
	iterator->cur_bucket = 0;
	iterator->cur_elem = hashtable->buckets[0].head->next;

	return iterator;
}

int iterator_destruct (ht_iterator_t iterator)
{
    if (iterator_is_ok (iterator) != IT_OK)
    {
        return HT_ERR;
    }

/*    if (iterator->cur_elem != NULL)
    {
        elem_destruct (iterator->cur_elem);
    }*/

    iterator->cur_elem = NULL;

    iterator->cur_bucket = -1;

    free (iterator); 

    return IT_OK;
}

static elem_t* ht_iterate (ht_iterator_t iterator)
{
	while(iterator->cur_elem == NULL)
	{
		if(iterator->cur_bucket < iterator->hashtable->max_hashvalue - 1)
		{
			iterator->cur_bucket++;
			iterator->cur_elem = iterator->hashtable->buckets[iterator->cur_bucket].head->next;
		}
		else
			return NULL;
	}

	elem_t* e = iterator->cur_elem;

	if(e != NULL)
	{
		iterator->cur_elem = e->next;
	}

	return e;
}

int iterator_get (ht_iterator_t iterator, char key[], char blob[], int* blob_size)
{
	if (blob_size == NULL)
	{
		return IT_NULL;
	}

	if (hashtable_error_catch (iterator_is_ok (iterator)) != HT_OK)
    {
        return HT_ERR;
    }

	elem_t* cur_elem = ht_iterate (iterator);

    if (cur_elem == NULL) 
    {
        return -1;
    }

	*blob_size = cur_elem->blob_size;

	strcpy (key, cur_elem->key);
	memmove (blob, cur_elem->blob, *blob_size);

	return IT_OK;
}

//==============================================================================
//------------------------------------------------------------------------------
//-----------------------------------TRICKS-------------------------------------
//------------------------------------------------------------------------------
//==============================================================================

static void* my_calloc (size_t num, size_t size)
{
	if (rand() % 100 < UNLUCK) return NULL;

	return calloc (num, size);
}

#undef DEBUG
#undef _CHECK_
#undef KEY_CHECK_
#undef BLOB_CHECK_
#undef INDEX_CHECK_
#undef HTEC
#undef LIST_CHECK_
#undef ELEM_CHECK_
#undef HASHTABLE_CHECK_
#undef ITERATOR_CHECK_
#undef OUT
#undef RETURN