//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// hashtable.h
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

typedef struct hashtable_t* hashtable_t;

typedef struct ht_iterator_t* ht_iterator_t;

hashtable_t hashtable_construct (unsigned int max_hashvalue, int* error_value);
int hashtable_clear (hashtable_t hashtable);
int hashtable_destruct (hashtable_t hashtable);
int hashtable_add (hashtable_t hashtable, char key[], char blob[], int blob_size);
int hashtable_del (hashtable_t hashtable, char key[]);

ht_iterator_t iterator_construct (hashtable_t hashtable, int* error_value);
int iterator_destruct (ht_iterator_t iterator);
int iterator_get (ht_iterator_t iterator, char key[], char blob[], int* blob_size);