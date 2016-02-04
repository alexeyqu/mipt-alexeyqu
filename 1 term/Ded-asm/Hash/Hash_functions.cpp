/** ***************************************************************************
 * @file    Hash_functions.cpp
 *
 * @ingroup hash
******************************************************************************/

#include "Hash.h"

#define HASH_CHECK_( hash )\
    if (hash == NULL)\
    {\
        if (0) (The_Log, ""HTML_TAB" \tHash is not allocated. <br>\n"); \
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
        asm (".intel_syntax noprefix\n"
             "mov eax, %1\n"
            "mov ecx, %2\n"
            "rol ecx, 1\n"
            "xor ecx, eax\n"
            "add eax, ecx\n"
            "xor eax, %2\n"
            "mov %0, eax\n"
            ".att_syntax noprefix\n"
            : "=a" (sum)
            : "a" (sum), "c" ((int) key[i]) );
//        sum += sum ^ ROL ((int) key[i], 1);
//        sum = sum ^ key[i];
    }

    sum /= strlen (key);

    *hash = sum;

    return HT_OK;
}

#undef HASH_CHECK_
