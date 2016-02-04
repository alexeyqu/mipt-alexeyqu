/** ***************************************************************************
 * @file    Tree_functions.cpp
******************************************************************************/

#include "Tree.h"

int Tree_Construct (root_t** new_tree)
{
    ASSERT (new_tree);

    *new_tree = (root_t*) calloc (1, sizeof (new_tree));

    (*new_tree)->size = 1;

    ISERR (Node_Construct (&(*new_tree)->root), (*new_tree)->root);

    TREE_CHECK (*new_tree);

    return TREE_OK;
}

int Tree_Destruct (root_t* tree)
{
    TREE_CHECK (tree);

    if (tree->root->left)  TEC (Node_Delete (tree, tree->root->left),  tree);
    if (tree->root->right) TEC (Node_Delete (tree, tree->root->right), tree);

    ISERR (Node_Destruct (tree->root), tree);

    tree->size = Poison;

    free (tree);

    return TREE_OK;
}

int Tree_Is_OK (root_t* tree)
{
    ASSERT (tree);

    int size = 0;

    if (Node_Is_OK (tree->root, &size) !=  NODE_OK) return TREE_ERR;

    if (size != tree->size) return TREE_ERR;

    return TREE_OK;
}

void Tree_Dump (FILE* outstream, root_t* tree)
{
    ASSERT (outstream);

    fprintf (outstream, "Dump of tree [%08x] - %s\n",
            (int) tree, (Tree_Is_OK (tree) == TREE_OK)? "ok" : "ERROR!");

    fprintf (outstream, "size = %d", tree->size);

    Node_Dump (outstream, tree->root, 0);

    fprintf (outstream, "\nDump of tree [%08x] finished.\n\n", (int) tree);
}
