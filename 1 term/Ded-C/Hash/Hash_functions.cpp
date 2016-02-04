/** ***************************************************************************
 * @file    Hash_functions.cpp
 *
 * @ingroup hash
******************************************************************************/

#include "Hash.h"

#define HASH_CHECK_( hash )\
    if (hash == NULL)\
    {\
        OUT (The_Log, ""HTML_TAB" \tHash is not allocated. <br>\n"); \
    }

int hash_1 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

    *hash = 0;

    return HT_OK;
}

int hash_2 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

    *hash = key[0];

    return HT_OK;
}

int hash_3 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

    long long sum = 0;

    unsigned int length = strlen (key);
    register unsigned int i = 0;
    for (i = 0; i < length; i++)
    {
        sum += key[i];
    }

    *hash = sum;

    return HT_OK;
}

int hash_4 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

    int sum = 0;

    unsigned int length = strlen (key);
    register unsigned int i = 0;
    for (i = 0; i < length; i++)
    {
        sum += key[i];
    }

    sum /= strlen (key);

    *hash = sum;

    return HT_OK;
}

int hash_5 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

    int sum = 0;

    unsigned int length = strlen (key);
    register unsigned int i = 0;
    for (i = 0; i < length; i++)
    {
        sum = key[i] ^ ROL ((int) sum, 1);
    }

    *hash = sum;

    return HT_OK;
}

int hash_6 (const char* key, int* hash)
{
    KEY_CHECK_ (key);
    HASH_CHECK_ (hash);

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

    return HT_OK;
}

#undef HASH_CHECK_
