/** ***************************************************************************
 * @file    Print_functions.cpp
******************************************************************************/

#include "Tree.h"

int Tree_Print (FILE* outstream, root_t* tree)
{
    ASSERT (outstream);
    TREE_CHECK (tree);

    int ret = Node_Print (outstream, tree->root);

    fprintf (outstream, "\n\n");

    return (ret == NODE_OK)? TREE_OK : TREE_ERR;
}

int Tree_Print_Postfix (FILE* outstream, root_t* tree)
{
    ASSERT (outstream);
    TREE_CHECK (tree);

    int ret = Node_Print_Postfix (outstream, tree->root);

    fprintf (outstream, "\n\n");

    return (ret == NODE_OK)? TREE_OK : TREE_ERR;
}

int Tree_Print_Dot (FILE* outstream, root_t* tree)
{
    ASSERT (outstream);
    TREE_CHECK (tree);

    fprintf (outstream, "digraph my_little_graph\n");
    fprintf (outstream, "{\n");

    int ret = Node_Print_Dot (outstream, tree->root);
    ASSERT (ret);

    fprintf (outstream, "}\n");

    return TREE_OK;
}

#define NODE_PRINT \
    switch (node->type)\
    {\
        case T_NUM:\
            fprintf (outstream, " %lg ", node->info);\
            break;\
\
        case T_OP:\
            switch ((int) node->info)\
            {\
                case ADD:\
                fprintf (outstream, " + ");\
                break;\
\
                case SUB:\
                fprintf (outstream, " - ");\
                break;\
\
                case MUL:\
                fprintf (outstream, " * ");\
                break;\
\
                case DIV:\
                fprintf (outstream, " / ");\
                break;\
\
                default:\
                fprintf (outstream, "ERROR!!!");\
                return NODE_ERR;\
            }\
            break;\
\
            default:\
            fprintf (outstream, "ERROR!!!");\
            return NODE_ERR;\
    }

int Node_Print (FILE* outstream, tree_t* node)
{
    ASSERT (outstream);
    NODE_CHECK (node);

    fprintf (outstream, "(");

    if (node->left) Node_Print (outstream, node->left);

    NODE_PRINT;

    if (node->right) Node_Print (outstream, node->right);

    fprintf (outstream, ")");

    return NODE_OK;
}

int Node_Print_Postfix (FILE* outstream, tree_t* node)
{
    ASSERT (outstream);
    NODE_CHECK (node);

    if (node->left) Node_Print_Postfix (outstream, node->left);
    if (node->right) Node_Print_Postfix (outstream, node->right);

    NODE_PRINT;

    return NODE_OK;
}

#define DOT_PRINT_NODE \
fprintf (outstream, "%d [label = ", (int) node);\
switch (node->type)\
    {\
        case T_NUM:\
            fprintf (outstream, " \"%lg\"", node->info);\
            break;\
\
        case T_OP:\
            switch ((int) node->info)\
            {\
                case ADD:\
                fprintf (outstream, " \"+\"");\
                break;\
\
                case SUB:\
                fprintf (outstream, " \"-\"");\
                break;\
\
                case MUL:\
                fprintf (outstream, " \"*\"");\
                break;\
\
                case DIV:\
                fprintf (outstream, " \"/\"");\
                break;\
\
                default:\
                fprintf (outstream, "ERROR!!!");\
                return NODE_ERR;\
            }\
            break;\
    }\
fprintf (outstream, "];\n");

int Node_Print_Dot (FILE* outstream, tree_t* node)
{
    ASSERT (outstream);
    NODE_CHECK (node);

    DOT_PRINT_NODE;

    if (node->left)
    {
        Node_Print_Dot (outstream, node->left);

        fprintf (outstream, "%d -> %d;\n", (int) node, (int) node->left);
    }
    if (node->right)
    {
        Node_Print_Dot (outstream, node->right);

        fprintf (outstream, "%d -> %d;\n", (int) node, (int) node->right);
    }

    return NODE_OK;
}
