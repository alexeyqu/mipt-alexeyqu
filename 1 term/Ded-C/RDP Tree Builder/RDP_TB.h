/** ***************************************************************************
 * @file    RDP_TB.h
******************************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define DEBUG 1

#ifndef DEBUG
    #error You should define DEBUG as 0 or 1!
#endif

#if (DEBUG == 1)
    #define OUT
    #define ASSERT( cond ) \
        if (!(cond))\
        {\
            printf ("Oops! Assertion <%s> failed \non %d line of %s file.\n", \
                    #cond, __LINE__, __FILE__);\
            exit (1);\
        }
    #define GRAM_ASSERT( cond ) \
        if (!(cond))\
        {\
            printf ("Oops! Assertion <%s> failed \non %d line of %s file.\n" \
                    "Please check the correspondance of input expression and used grammar.\n", \
                    #cond, __LINE__, __FILE__);\
            printf ("Current symbol: '%c'\n", *s);\
            exit (1);\
        }
#else
    #define OUT if (0)
    #define ASSERT( cond )
#endif

#define ISERR( func, object ) \
    if (Exception_Catcher (func, "", object) == ERR) exit (1);

#define NODE_CHECK( node ) \
    ISERR (Node_Is_OK (node, NULL), node);

#define TREE_CHECK( tree ) \
    ISERR (Tree_Is_OK (tree), tree);

const unsigned int Poison    = 0xDEAD; //!< Poison value
const unsigned int Set_value = 0xADD; //!< Start value
extern char* s; //!< Expression's iterator
const int Max_Len = 100; //!< Maximum length of expression
extern FILE* The_Log; //!< Log file descriptor

typedef struct tree_t
{
    double info; //!< Info in current node
    int size; //!< Size of the subtree
    int type; //!< Type of current node

    tree_t* left;
    tree_t* right;
    tree_t* parent;
} tree_t;

typedef struct root_t
{
    int size; //!< Size of the whole tree

    tree_t* root;
} root_t;

enum NODE_ERR
{
    NODE_ERR      = -1000,
    NODE_OK       = 1000,
    NODE_NULL     = 1001,
    NODE_WR_SIZE  = 1002,
    NODE_WR_CNCTS = 1003
};

enum TREE_ERR
{
    TREE_ERR     = -2000,
    TREE_OK      = 2000,
    TREE_NULL    = 2001,
    TREE_WR_SIZE = 2002
};

enum GLOBAL_ERR
{
    ERR = -1,
    OK  = 0
};

enum NODE_TYPE
{
    NO_TYPE = 0
};

int  Node_Construct     (tree_t** new_node);
int  Node_Destruct      (tree_t* node);
int  Node_Is_OK         (tree_t* node, int* size);
void Node_Dump          (FILE* outstream, tree_t* node, unsigned int depth);

int  Tree_Construct     (root_t** new_tree);
int  Tree_Destruct      (root_t* tree);
int  Tree_Is_OK         (root_t* tree);
void Tree_Dump          (FILE* outstream, root_t* tree);

/** ***************************************************************************
 * Node_Add_Left
 *
 * @brief Adds node as left child
 *
 * @ingroup node
 *
 * @param[out]  tree    pointer to tree
 * @param[out]  node    pointer to parent node
 * @param       child   pointer to child node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Add_Left     (root_t* tree, tree_t* node, tree_t* child);

/** ***************************************************************************
 * Node_Add_Right
 *
 * @brief Adds node as right child
 *
 * @ingroup node
 *
 * @param[out]  tree    pointer to tree
 * @param[out]  node    pointer to parent node
 * @param       child   pointer to child node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Add_Right    (root_t* tree, tree_t* node, tree_t* child);

/** ***************************************************************************
 * Node_Delete
 *
 * @brief Deletes subtree from tree
 *
 * @ingroup node
 *
 * @param[out]  tree    pointer to tree
 * @param[out]  node    pointer to node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Delete       (root_t* tree, tree_t* node);

/** ***************************************************************************
 * Tree_Update_Sizes
 *
 * @brief Updates info about sizes in nodes of the tree
 *
 * @ingroup tree
 *
 * @param[out]  tree    pointer to tree
 * @param[out]  node    pointer to node
 * @param       d_size  delta size
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Update_Sizes (root_t* tree, tree_t* node, int d_size);

/** ***************************************************************************
 * Node_Print
 *
 * @brief Prints node infixly :)
 *
 * @ingroup print
 * @ingroup node
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  node        pointer to node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Print         (FILE* outstream, tree_t* node);

/** ***************************************************************************
 * Node_Print_Postfix
 *
 * @brief Prints node postfixly :)
 *
 * @ingroup print
 * @ingroup node
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  node        pointer to node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Print_Postfix (FILE* outstream, tree_t* node);

/** ***************************************************************************
 * Node_Print
 *
 * @brief Prints node dotfixly :)
 *
 * @ingroup print
 * @ingroup node
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  node        pointer to node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Node_Print_Dot     (FILE* outstream, tree_t* node);

/** ***************************************************************************
 * Tree_Print
 *
 * @brief Prints tree infixly :)
 *
 * @ingroup print
 * @ingroup tree
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  tree        pointer to tree
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Print         (FILE* outstream, root_t* tree);

/** ***************************************************************************
 * Tree_Print_Postfix
 *
 * @brief Prints tree postfixly :)
 *
 * @ingroup print
 * @ingroup tree
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  tree        pointer to tree
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Print_Postfix (FILE* outstream, root_t* tree);

/** ***************************************************************************
 * Tree_Print_Dot
 *
 * @brief Prints tree dotfixly :)
 *
 * @ingroup print
 * @ingroup tree
 *
 * @param[out]  outstream   output file descriptor
 * @param[out]  tree        pointer to tree
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Print_Dot     (FILE* outstream, root_t* tree);

/** ***************************************************************************
 * Get_G5
 *
 * @brief Gets G5 expression
 * Grammar G5:
 * G5
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
double Get_G5 (char* str);

int Exception_Catcher (int err_code, const char* msg, ...);
