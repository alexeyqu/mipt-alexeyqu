/** ***************************************************************************
 * @file    Read_functions.cpp
******************************************************************************/

#include "Tree.h"

int Tree_Read (FILE* instream, root_t* tree, tree_t* node)
{
    ASSERT (instream);
    TREE_CHECK (tree);
    NODE_CHECK (node);

    char c = 0;
    int res_input = 0;
    int cur_type = 0;

    res_input = fscanf (instream, " %c", &c);
    ASSERT (res_input); ASSERT (c == '(');

    TEC (Tree_Get_Token (instream, node, &cur_type), node);

    if (cur_type == T_NUM)
    {
        node->left = NULL;
        node->right = NULL;

        res_input = fscanf (instream, " %c", &c);
        ASSERT (res_input); ASSERT (c == ')');

        return TREE_OK;
    }

    ASSERT (cur_type == T_L_BR);

    tree_t* new_node_left = NULL, *new_node_right = NULL;

    TEC (Node_Construct (&new_node_left),                new_node_left);

    TEC (Node_Add_Left  (tree, node, new_node_left),     node);

    TEC (Tree_Read      (instream, tree, new_node_left), tree);

    TEC (Tree_Get_Token (instream, node, &cur_type),     node);

    ASSERT (cur_type == T_OP);

    TEC (Node_Construct (&new_node_right),                new_node_right);

    TEC (Node_Add_Right (tree, node, new_node_right),     node);

    TEC (Tree_Read      (instream, tree, new_node_right), tree);

    res_input = fscanf (instream, " %c", &c);
    ASSERT (res_input); ASSERT (c == ')');

    return TREE_OK;
}

int Tree_Get_Token (FILE* instream, tree_t* node, int* cur_type)
{
    ASSERT (instream);
    ASSERT (node);
    ASSERT (cur_type);

    int res_input = 0;

    if (Tree_Scan_For_Number (instream) == 1)
    {
        double number = 0;
        res_input = fscanf (instream, " %lg", &number);
        ASSERT (res_input);

        node->info = number;
        node->type = T_NUM;
        *cur_type = T_NUM;

        return TREE_OK;
    }
    else
    {
        char c = 0;

        res_input = fscanf (instream, " %c", &c);
        ASSERT (res_input);

        switch (c)
        {
            case ADD:
            case SUB:
            case MUL:
            case DIV:
                node->type = T_OP;
                node->info = (double) c;
                *cur_type = T_OP;
                return TREE_OK;

            case L_BR:
                *cur_type = T_L_BR;
                ungetc (c, instream);
                return TREE_OK;

            case R_BR:
                *cur_type = T_R_BR;
                ungetc (c, instream);
                return TREE_OK;

            default:
                fprintf (stdout, "Wrong token %c\n", c);
                return TREE_ERR;
        }
    }
}

int Tree_Scan_For_Number (FILE* instream)
{
    ASSERT (instream);

    int c = 0;

    while (isspace (c = getc (instream)));

    if (isdigit (c))
    {
        ungetc (c, instream);

        return 1;
    }

    int c1 = getc (instream);

    if ((c == '+' || c == '-') && isdigit (c1))
    {
        ungetc (c1, instream);
        ungetc (c, instream);

        return 1;
    }

    ungetc (c1, instream);
    ungetc (c, instream);

    return 0;
}
