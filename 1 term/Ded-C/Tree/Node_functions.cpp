/** ***************************************************************************
 * @file    Node_functions.cpp
******************************************************************************/

#include "Tree.h"

int Node_Construct (tree_t** new_node)
{
    ASSERT (new_node);

    *new_node = (tree_t*) calloc (1, sizeof (tree_t));

    (*new_node)->info   = Set_value;
    (*new_node)->size   = 1;
    (*new_node)->type   = NO_TYPE;
    (*new_node)->left   = NULL;
    (*new_node)->right  = NULL;
    (*new_node)->parent = NULL;

    NODE_CHECK (*new_node);

    return NODE_OK;
}

int Node_Destruct (tree_t* node)
{
    NODE_CHECK (node);

    node->info = Poison;
    node->size = Poison;
    node->type = Poison;

    node->left   = NULL;
    node->right  = NULL;
    node->parent = NULL;

    free (node);

    return NODE_OK;
}

int Node_Is_OK (tree_t* node, int* size)
{
    if (node == NULL) return NODE_NULL;

    if (size == NULL)
    {
        int s = 0;
        size = &s;
    }

    int size_other = *size;

    (*size)++;

    if (node->left)
    {
        if (node->left->parent != node) return NODE_WR_CNCTS;

        if (Node_Is_OK (node->left, size) != NODE_OK) return NODE_ERR;
    }

    if (node->right)
    {
        if (node->right->parent != node) return NODE_WR_CNCTS;

        if (Node_Is_OK (node->right, size) != NODE_OK) return NODE_ERR;
    }

    if (node->size != *size - size_other) return NODE_WR_SIZE;

    return NODE_OK;
}

#define FORMAT_DUMP \
    fprintf (outstream, "\n");\
    for (i = 0; i < depth; i++)\
    {\
        fprintf (outstream, "\t");\
    }

void Node_Dump (FILE* outstream, tree_t* node, unsigned int depth)
{
    ASSERT (outstream); ASSERT (node);

    register unsigned int i = 0;

    FORMAT_DUMP;
    fprintf (outstream, "(");

    if (node->left)
    {
        Node_Dump (outstream, node->left, depth + 1);
    }
    else
    {
        FORMAT_DUMP;
        fprintf (outstream, "\tnil");
    }

    FORMAT_DUMP;
    fprintf (outstream, "\t%lg | Size: %d | Address: [%08x], Left: [%08x], Right: [%08x] "
            "Parent: [%08x], Type: %d, %s",
            node->info, node->size,
            (int) node, (int) node->left, (int) node->right,
            (int) node->parent, node->type,
            (Node_Is_OK (node, NULL) == NODE_OK)? "ok" : "ERROR!");

    if (node->right)
    {
        Node_Dump (outstream, node->right, depth + 1);
    }
    else
    {
        FORMAT_DUMP;
        fprintf (outstream, "\tnil");
    }

    FORMAT_DUMP;
    fprintf (outstream, ")");
}

#undef FORMAT_DUMP

int Node_Add_Left (root_t* tree, tree_t* node, tree_t* child)
{
    TREE_CHECK (tree);
    NODE_CHECK (node);
    NODE_CHECK (child);

    if (node->left == NULL)
    {
        node->left = child;
        node->left->parent = node;

        TEC (Tree_Update_Sizes (tree, node, child->size), node);
    }

    Tree_Dump (stdout, tree);

    TREE_CHECK (tree);
    NODE_CHECK (node);

    return TREE_OK;
}

int Node_Add_Right (root_t* tree, tree_t* node, tree_t* child)
{
    TREE_CHECK (tree);
    NODE_CHECK (node);
    NODE_CHECK (child);

    if (node->right == NULL)
    {
        node->right = child;
        node->right->parent = node;

        TEC (Tree_Update_Sizes (tree, node, child->size), node);
    }

    TREE_CHECK (tree);
    NODE_CHECK (node);

    return TREE_OK;
}

int Node_Delete (root_t* tree, tree_t* node)
{
    TREE_CHECK (tree);
    NODE_CHECK (node);

    if (node->left)
    {
        TEC (Node_Delete (tree, node->left), node->left);
    }

    if (node->right)
    {
        TEC (Node_Delete (tree, node->right), node->right);
    }

    if (node->parent->left == node) node->parent->left = NULL;
    if (node->parent->right == node) node->parent->right = NULL;

    Tree_Update_Sizes (tree, node->parent, -1);

    TEC (Node_Destruct (node), node);

    TREE_CHECK (tree);

    return TREE_OK;
}

int Tree_Update_Sizes (root_t* tree, tree_t* node, int d_size)
{
    ASSERT (tree); ASSERT (node);

    tree_t* cur_tree = node;

    while (cur_tree != tree->root)
    {
        cur_tree->size += d_size;

        cur_tree = cur_tree->parent;
    }

    tree->root->size += d_size;
    tree->size       += d_size;

    return TREE_OK;
}
