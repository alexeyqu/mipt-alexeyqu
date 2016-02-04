/** ***************************************************************************
 * @file    BT_main.cpp
 * @mainpage
 * @date    2014-25-5
 * @author  Alexey Kulikov <alexeyqu@gmail.com>, group 376, DCAM MIPT
 * @version 1.00
 *
 * Binary translator.
 *
 * @par     Translating commands summary (V1.14):
 *          : {value}       - sets label {value}\n
 *          PROC : {name}   - sets pointer to function {name}\n
 *        + PUSH {value\reg}- pushes value\reg to stack\n
 *        + POP (reg)       - pops value out of stack (or to reg)\n
 *          IN              - gets value from the input stream and pushes it into stack\n
 *        ! OUT             - prints popped value on screen\n
 *        + DUP             - duplicates top value of stack\n\n
 *
 *        + ADD             - pushes sum of 2 popped elements\n
 *        + SUB             - pushes difference of 2 popped elements\n
 *        + MUL             - pushes product of 2 popped elements\n
 *        + DIV             - pushes quotient of 2 popped elements\n
 *          POW             - pushes power function of 2 popped elements\n
 *          SIN             - pushes sin of last element instead of it\n
 *          COS             - pushes cos of last element instead of it\n
 *          SQRT            - pushes sqrt of last element instead of it\n\n
 *
 *        + AND             - logical and\n
 *        + OR              - logical or\n
 *        + NOT             - logical not\n\n
 *
 *        ? EQU             - ==\n
 *        ? LT              - <\n
 *        ? GT              - >\n\n
 *
 *        ? JMP :{value}    - jumps on {value} label\n
 *        ? JE :{value}     - jumps on {value} label if pop = pop\n
 *        ? JNE :{value}    - jumps on {value} label if pop != pop\n
 *        ? JA :{value}     - jumps on {value} label if pop > pop\n
 *        ? JAE :{value}    - jumps on {value} label if pop >= pop\n
 *        ? JB :{value}     - jumps on {value} label if pop < pop\n
 *        ? JBE :{value}    - jumps on {value} label if pop <= pop\n\n
 *
 *        ? CALL :{name}    - calls {name} function\n
 *        ? RET             - returns from current function\n\n
 *
 *          SPC             - prints ' '\n
 *          TAB             - prints '\t'\n
 *          EOL             - prints '\n'\n\n
 *
 *          DUMP            - prints full dump info on CPU\n
 *          MSG "text"      - printf text\n\n
 *
 *          // {text}       - comment\n\n
 *
 *          END             - ends the program\n
 *
 * @par     V1.00
 *          - Translation from CPU syntax V1.00 (most basic instructions)
******************************************************************************/

#include "BT.h"

extern FILE* The_Log;

int main()
{
    fprintf (stdout, "Hello! I'm a smart binary translator.\n");
    fprintf (stdout, "I take assembled code from the CPU project\n"
            "and transform it into executable elf file.\n");
    fprintf (stdout, "-----\n");
    fprintf (stdout, "(c) AQ, <alexeyqu@gmail.com>, DCAM MIPT, gr. 376\n\n");

    fprintf (stdout, "Please enter name of the file we're going to generate: ");

    char elfname[Max_Name_Len] = {};
    int res_input = 0;
    int nofnops = 0;

    res_input = fscanf (stdin, "%s", elfname);
    assert (res_input);

    BT_Empty_File_Generate (elfname, &nofnops);

    FILE* elfile = fopen (elfname, "r+");
    assert (elfile);

    char cpuname[Max_Name_Len] = {};

    sprintf (cpuname, "code.cpu");

    FILE* cpufile = fopen (cpuname, "r");
    assert (cpufile);

    BT_Translate (elfile, cpufile, nofnops);

    fclose (cpufile);
    fclose (elfile);

    return 0;
}
