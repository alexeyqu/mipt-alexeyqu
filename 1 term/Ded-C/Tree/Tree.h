/** ***************************************************************************
 * @file    Tree.h
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
            fprintf (stdout, "Oops! Assertion <%s> failed \non %d line of %s file.\n", \
                    #cond, __LINE__, __FILE__);\
            exit (1);\
        }
#else
    #define OUT if (0)
    #define ASSERT( cond )
#endif

#define NODE_CHECK( node ) \
    TEC (Node_Is_OK (node, NULL), node);

#define TREE_CHECK( tree ) \
    TEC (Tree_Is_OK (tree), tree);

#define TEC( func, object )\
    if (Tree_Error_Handler (func, "", object) != TREE_OK) return TREE_ERR;

const unsigned int Poison    = 0xDEAD; //!< Poison value
const unsigned int Set_value = 0xADD; //!< Start value
extern FILE* The_Log; //!< Log file descriptor

typedef struct tree_t
{
    double info; //!< Info in current node
    int size; //!< Size of subtree
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

enum NODE_TYPE
{
    NO_TYPE = 0,
    T_NUM   = 1,
    T_OP    = 2,
    T_L_BR  = 3,
    T_R_BR  = 4
};

enum NODE_INFO
{
    ADD  = '+',
    SUB  = '-',
    MUL  = '*',
    DIV  = '/',
    L_BR = '(',
    R_BR = ')'
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
 * Tree_Read
 *
 * @brief Reads a tree from a full-bracket expression
 *
 * @ingroup read
 * @ingroup tree
 *
 * @param[out]  instream    input file descriptor
 * @param[out]  tree        pointer to tree
 * @param[out]  node        pointer to node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Read            (FILE* instream, root_t* tree, tree_t* node);

/** ***************************************************************************
 * Tree_Get_Token
 *
 * @brief Gets a token from the input stream
 *
 * @ingroup read
 * @ingroup node
 *
 * @param[out]  instream    input file descriptor
 * @param[out]  node        pointer to node
 * @param[out]  cur_type    current type of node
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Get_Token       (FILE* instream, tree_t* node, int* cur_type);

/** ***************************************************************************
 * Tree_Scan_For_Number
 *
 * @brief Looks for a number in the input stream
 *
 * @ingroup read
 *
 * @param[out]  instream    input file descriptor
 *
 * @retval  0/1 - if there is a number
******************************************************************************/
int  Tree_Scan_For_Number (FILE* instream);

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
 * Tree_Error_Handler
 *
 * @brief Catches errors..
 *
 * @ingroup tree
 *
 * @param   err_code    code of error
 * @param   msg         message to print
 *
 * @retval  ERR_LIST
******************************************************************************/
int  Tree_Error_Handler (int err_code, const char* msg, ...);
