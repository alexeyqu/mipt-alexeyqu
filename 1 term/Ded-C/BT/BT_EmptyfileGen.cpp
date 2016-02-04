#include "BT.h"

void BT_Empty_File_Generate (char elfname[], int* nofnops)
{
    char asmname[Max_Name_Len] = {};

    strcpy (asmname, elfname);
    strcat (asmname, ".asm");

    FILE* nasmfile = fopen (asmname, "w");
    assert (nasmfile);

    fprintf (nasmfile, "global _start\n\n"
                       "section .data\n\n"
                       "version db \"%d\"\n\n"
                       "section .text\n\n"
                       "_start:\n"
                       "\t mov eax, 4\n"
                       "\t mov ebx, 1\n"
                       "\t mov ecx, version\n"
                       "\t mov edx, 4\n"
                       "\t int 80h\n\n"
                       , BT_Version);

    fprintf (stdout, "Please set .model of file "
                     "(t = test, s = small, m = medium, l = large): ");

    char model = 0;
    int res_input = 0;

    res_input = fscanf (stdin, " %c", &model);
    assert (res_input);

    switch (model)
    {
        case 't':
            *nofnops = 100;
            break;

        case 's':
            *nofnops = 1000;
            break;

        case 'm':
            *nofnops = 2000;
            break;

        case 'l':
            *nofnops = 5000;
            break;

        case 'h':
            *nofnops = 10000;
            break;

        default:
            assert (!("Bad .model!\n"));
            break;
    }

    register int i = 0;
    for (i = 0; i < *nofnops; i++)
    {
        fprintf (nasmfile, "\t nop\n");
    }

    fprintf (nasmfile, "\nFinish:\n"
                       "\t mov eax, 1\n"
                       "\t mov ebx, 0\n"
                       "\t int 80h");

    fclose (nasmfile);

    char nasm_request[Max_Str_Len] = {};
    char ld_request[Max_Str_Len]   = {};
    char ddd_request[Max_Str_Len]   = {};

    sprintf (nasm_request, "nasm -f elf -F stabs %s", asmname);
    sprintf (ld_request,   "ld %s.o -o %s", elfname, elfname);
    sprintf (ddd_request,   "ddd %s &", elfname);

    system (nasm_request);
    system (ld_request);
//    system (ddd_request);

    return;
}
