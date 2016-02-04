#include "BT.h"

void BT_Translate (FILE* elfile, FILE* cpufile, int nofnops)
{
    assert (elfile);
    assert (cpufile);

    fseek (elfile,  0, 0);
    fseek (cpufile, 0, 0);

    unsigned int c = 0;
    int counter     = 0;
    int res_input   = 0;
    unsigned int ip = 0;

    while (counter < 5)
    {
        res_input = fread (&c, sizeof (int), 1, elfile);
        if (res_input == EOF) assert (!"No nops!");

        ip += 4;

//        printf ("0x%08x\n", c);
//        getchar();

//        bin_printf (stdout, c);
//        getchar();

        if (c == 0x90909090) counter++;
        else counter = 0;
    }

    printf ("0x%08x\n", ip);

    printf ("Gotcha!\n");

    int curcmd     = 0;
    int curnasmcmd = 0;
    int arg        = 0;
    int cpuip      = 1;
    int asmip      = 0;

    int iptranslation[Max_Code_Len] = {};

    while (1)
    {
        res_input = fscanf (cpufile, "%d", &curcmd);
        if (res_input == EOF) break;

        iptranslation[cpuip] = asmip;

        switch (curcmd)
        {
            case CMD_PUSH:

                cpuip += 2;
                asmip += 5;

                break;

            case CMD_PUSH_AX:

                cpuip++;
                asmip++;

                break;

            case CMD_PUSH_BX:

                cpuip++;
                asmip++;

                break;

            case CMD_PUSH_CX:

                cpuip++;
                asmip++;

                break;

            case CMD_PUSH_DX:

                cpuip++;
                asmip++;

                break;

            case CMD_POP:

                cpuip++;
                asmip += 3;

                break;

            case CMD_POP_AX:

                cpuip++;
                asmip++;

                break;

            case CMD_POP_BX:

                cpuip++;
                asmip++;

                break;

            case CMD_POP_CX:

                cpuip++;
                asmip++;

                break;

            case CMD_POP_DX:

                cpuip++;
                asmip++;

                break;

            case CMD_ADD:

                cpuip++;
                asmip += 5;

                break;

            case CMD_SUB:

                cpuip++;
                asmip += 5;

                break;

            case CMD_MUL:

                cpuip++;
                asmip += 11;

                break;

            case CMD_DIV:

                cpuip++;
                asmip += 17;

                break;

            case CMD_DUP:

                cpuip++;
                asmip += 3;

                break;

            case CMD_AND:

                cpuip++;
                asmip += 5;

                break;

            case CMD_OR:

                cpuip++;
                asmip += 5;

                break;

            case CMD_NOT:

                cpuip++;
                asmip += 4;

                break;

            case CMD_JMP:

                cpuip += 2;
//                asmip += 5;

                break;

             case CMD_JAE:

                cpuip += 2;
                asmip += 4;

                break;
        }
    }

    fseek (cpufile, 0, 0);

    #define WRITECMD( value ) \
        { \
                curnasmcmd = value; \
                fwrite (&curnasmcmd, sizeof (char), 1, elfile); \
                ip++; \
        }

    while (1)
    {
        res_input = fscanf (cpufile, "%d", &curcmd);
        if (res_input == EOF) break;

        switch (curcmd)
        {
            case CMD_PUSH:

                WRITECMD (0x68); // push

                res_input = fscanf (cpufile, "%d", &arg);
                assert (res_input);

                fwrite (&arg, sizeof (char), 4, elfile);
                ip += 4;

                break;

            case CMD_PUSH_AX:

                WRITECMD (0x50); // push ax

                break;

            case CMD_PUSH_BX:

                WRITECMD (0x53); // push bx

                break;

            case CMD_PUSH_CX:

                WRITECMD (0x51); // push cx

                break;

            case CMD_PUSH_DX:

                WRITECMD (0x52); // push dx

                break;

            case CMD_POP:

                WRITECMD (0x04); // add esp, 4
                WRITECMD (0xC4);
                WRITECMD (0x83);

                break;

            case CMD_POP_AX:

                WRITECMD (0x58); // pop ax

                break;

            case CMD_POP_BX:

                WRITECMD (0x5B); // pop bx

                break;

            case CMD_POP_CX:

                WRITECMD (0x59); // pop cx

                break;

            case CMD_POP_DX:

                WRITECMD (0x5A); // pop dx

                break;
/*
            case CMD_IN:

                curnasmcmd = 0x04EC83; // sub esp, 4

                fwrite (&curnasmcmd, sizeof (char), 3, elfile);

                curnasmcmd = 0xB8; // mov ax
                arg = 0x3;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xBB; // mov bx
                arg = 0x1;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xE189; // mov cx, sp

                fwrite (&curnasmcmd, sizeof (char), 2, elfile);

                curnasmcmd = 0xBA; // mov ax
                arg = 0x4;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xCD; // int 80h
                arg = 0x80;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 1, elfile);

                break;

            case CMD_OUT:
                curnasmcmd = 0x04C483; // add esp, 4

                fwrite (&curnasmcmd, sizeof (char), 3, elfile);

                curnasmcmd = 0xB8; // mov ax
                arg = 0x1;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xBB; // mov bx
                arg = 0x1;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xE189; // mov cx, sp

                fwrite (&curnasmcmd, sizeof (char), 2, elfile);

                curnasmcmd = 0xBA; // mov ax
                arg = 0x4;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 4, elfile);

                curnasmcmd = 0xCD; // int 80h
                arg = 0x80;

                fwrite (&curnasmcmd, sizeof (char), 1, elfile);
                fwrite (&arg,        sizeof (char), 1, elfile);

                break;
*/
            case CMD_ADD:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                curnasmcmd = 0xFE01; // add esi, edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x56); // push esi

                break;

            case CMD_SUB:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                curnasmcmd = 0xFE29; // sub esi, edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x56); // push esi

                break;

            case CMD_MUL:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                WRITECMD (0x50); // push eax

                curnasmcmd = 0xF089; // mov eax, esi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xE7F7; // mul edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xC689; // mov esi, eax
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x58); // pop eax

                WRITECMD (0x56); // push esi

                break;

            case CMD_DIV:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                WRITECMD (0x50); // push eax

                WRITECMD (0x52); // push edx

                curnasmcmd = 0xF089; // mov eax, esi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xD231; // xor edx
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xF7F7; // div edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xF631; // xor esi, esi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0xC689; // mov esi, eax
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x5A); // pop edx

                WRITECMD (0x58); // pop eax

                WRITECMD (0x56); // push esi

                break;

            case CMD_DUP:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x56); // push esi

                WRITECMD (0x56); // push esi

                break;

            case CMD_AND:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                curnasmcmd = 0xFE21; // and esi, edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x56) // push esi

                break;

            case CMD_OR:

                WRITECMD (0x5E); // pop esi

                WRITECMD (0x5F); // pop edi

                curnasmcmd = 0xFE09; // or esi, edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x56) // push esi

                break;

            case CMD_NOT:

                WRITECMD (0x5E); // pop esi

                curnasmcmd = 0xF7D6; // not esi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                WRITECMD (0x56) // push esi

                break;

            case CMD_JMP:

                WRITECMD (0xE9); // jmp

                res_input = fscanf (cpufile, "%d", &arg);
                assert (res_input);

                curnasmcmd = iptranslation[arg]; // get addr
                fwrite (&curnasmcmd, sizeof (char), 4, elfile);
                ip += 4;

                break;

            case CMD_JAE:

                WRITECMD (0x5E);
                WRITECMD (0x5F);

                curnasmcmd = 0xFE39; // cmp esi, edi
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                curnasmcmd = 0x830F; // jae
                fwrite (&curnasmcmd, sizeof (char), 2, elfile);
                ip += 2;

                res_input = fscanf (cpufile, "%d", &arg);
                assert (res_input);

                curnasmcmd = iptranslation[arg]; // get addr
                fwrite (&curnasmcmd, sizeof (char), 4, elfile);
                ip += 4;

                break;
        }

        #undef WRITECMD

        printf ("0x%08x\n", ip);
    }

    return;
}
