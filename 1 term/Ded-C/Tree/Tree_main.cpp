/** ***************************************************************************
 * @file    Tree_main.cpp
 * @mainpage
 * @date    2013-12-27 00:03
 * @author  Alexey Kulikov <alexeyqu@gmail.com>, group 376, DCAM MIPT
 * @version 1.01
 *
 * Binary tree.
 *
 * @par     V1.00
 *          - Realised binary tree data stucture
 *          - Added basic functions (e.g. construct, destruct etc)
 *          - Added add_left, add_right and delete subtree functions
 *          - Added printing of a full-bracket expression into .txt file
 *
 * @par     V1.01
 *          - Printing into .dot file now featured
 *          - Code is doxypatched now
 *
 * @defgroup    tree
 * @defgroup    node
 * @defgroup    read
 * @defgroup    print
******************************************************************************/

#include "Tree.h"

int main()
{
    time_t t_now = time (0);

    struct tm* now = localtime (&t_now);

    fprintf (stdout, "Hello!\n");
    fprintf (stdout, "I'm a simple program that demonstrates \n"
            "basic functions for working with binary tree.\n");
    fprintf (stdout, "I can read a full-bracket expression (e.g. ((2)+(3)) ),\n");
    fprintf (stdout, "build a binary tree that corresponds to this expression\n");
    fprintf (stdout, " and print it back into .txt or .dot file.\n");
    fprintf (stdout, "!!!\n");
    fprintf (stdout, "Please pay attention to these _integer_ values:\n");
    fprintf (stdout, "%d - Start value\n", Set_value);
    fprintf (stdout, "%d - Poison value\n", Poison);
    fprintf (stdout, "!!!\n");
    fprintf (stdout, "Author: AQ\n");
    fprintf (stdout, "Compiled from %s file\n"
            "on %s %s\n", __FILE__, __DATE__, __TIME__);
    fprintf (stdout, "Executed on %d-%d-%d %d:%d:%d\n", now->tm_mon + 1, now->tm_mday, now->tm_year + 1900,
            now->tm_hour, now->tm_min, now->tm_sec);
    fprintf (stdout, "--------------------\n\n");

    FILE* instream  = fopen ("input.txt",  "r");
    FILE* outstream = fopen ("output.txt", "w");
    FILE* dotstream = fopen ("output.dot", "w");
    FILE* The_Log   = fopen ("log.txt",    "w");

    ASSERT (instream); ASSERT (outstream); ASSERT (dotstream); ASSERT (The_Log);

    root_t* my_tree = NULL;

    TEC (Tree_Construct (&my_tree), my_tree);

    fprintf (stdout, "Reading the expression and constructing the tree..\n");

    TEC (Tree_Read (instream, my_tree, my_tree->root), my_tree);

    fprintf (stdout, "Printing & dumping the tree..\n");

    TEC (Tree_Print         (outstream, my_tree), my_tree);
    TEC (Tree_Print_Postfix (outstream, my_tree), my_tree);
    TEC (Tree_Print_Dot     (dotstream, my_tree), my_tree);

    Tree_Dump (outstream, my_tree);

    TEC (Tree_Destruct (my_tree), my_tree);

    fprintf (stdout, "Done!\n");

    fclose (The_Log);
    fclose (instream);
    fclose (outstream);
    fclose (dotstream);

    system ("xdot output.dot");

    return 0;
}
