/** ***************************************************************************
 * @file    Get_G4.cpp
******************************************************************************/

#include "RDP_TB.h"

double Get_G5 (char* str)
{
    ASSERT (str);

    printf ("G5.\n");

    s = str;

    double res; //= Get_E_G4();

    GRAM_ASSERT (*s == 0);

    return res;
}

int Exception_Catcher (int err_code, const char* msg, ...)
{
    fprintf (stdout, "%s", msg);

    switch (err_code)
    {
        case NODE_NULL:
            printf ("Node is not allocated.\n");
            break;

        case NODE_WR_CNCTS:
            printf ("Wrong connections!\n");
            break;

        case NODE_WR_SIZE:
            printf ("Wrong size of subtree!\n");
            break;

        case TREE_NULL:
            printf ("Tree is not allocated.\n");
            break;

        case TREE_WR_SIZE:
            printf ("Wrong size of tree!\n");
            break;

        case TREE_OK:
        case NODE_OK:
            return OK;

        case TREE_ERR:
        case NODE_ERR:
            printf ("ERROR!");
            exit (1);
            break;

        default:
            printf ("Wrong error code! %d :)\n", err_code);
            break;
    }

    getchar();
    return ERR;
}
