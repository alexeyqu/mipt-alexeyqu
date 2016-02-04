/** ***************************************************************************
 * @file    Stack.cpp
 * @mainpage
 * @date    2013-11-21
 * @author  Alexey Kulikov <alexeyqu@gmail.com>, group 376, DCAM MIPT
 * @version 1.05
 *
 * Stack usage.
 *
 * @par     This program is supposed to demostrate realised stack data structure
 *          and functions operating with it.
 *
 * @par     V1.00
 *          * Nothing but "Hello, stack" realised.
 *
 * @par     V1.01
 *          * Realised stack functions:
 *              - Stack_Construct
 *              - Stack_Destruct
 *              - Stack_Is_Empty
 *              - Stack_Is_Full
 *              - Stack_Is_OK
 *              - Stack_Push
 *              - Stack_Pop
 *              - Stack_Dump
 *          * Realised special function for full testing of stack functions
 *          * Added some interface
 *          * The code is divided by files
 *          * Added doxygen comments
 *
 * @par     V1.02
 *          * Added following stack functions (part of them are just optimized versions):
 *              - Need_More_Memory
 *              - Stack_Perverted_Push
 *              - Stack_Perverted_Pop
 *              - Stack_Space_Cpy
 *          * Realised dynamic model of stack objects
 *
 * @par     V1.03
 *          * Stack is operating with double precision values now
 *          * CMD_'s frame added
 *
 * @par     V1.04
 *          * Error catcher added
 *          * ERRORS framework added
 *
 * @par     V1.05
 *          * Final code review
******************************************************************************/

#include "Stack.h"

int main()
{
    printf ("Program compiled from %s \non %s %s.\n", __FILE__, __TIME__, __DATE__);
    printf ("Author: Alexey Kulikov\n");
    printf ("Version: %lg\n\n", VERSION);
    printf ("Hello, dear user!\n");
    printf ("I'm a program that demonstrates usage of data structure called stack.\n");
    printf ("Realised functions:\n");
    printf ("- Construct stack\n"
            "- Dectruct stack\n"
            "- Push value to stack\n"
            "- Pop top value from stack\n"
            "- Output dump info about stack output\n"
            "- Check if stack is full/empty\n"
            "- Check if stack is ok\n"
            "- and smth extra - see the docs\n");
    printf ("That's all!\n");

    Stack_Test();

    return 0;
}
