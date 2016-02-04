#!/bin/bash
gcc -Wall -Werror -fprofile-arcs -ftest-coverage -O0 -o hashtable testht.c hashtable.c
./hashtable
gcov hashtable.c testht.c

exit 0