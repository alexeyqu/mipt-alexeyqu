/** ***************************************************************************
 * @file    RDP.h
******************************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
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

extern FILE* The_Log; //!< Log file descriptor
extern char* s; //!< Expression's iterator
const int Max_Len = 100; //!< Maximum length of expression

/** ***************************************************************************
 * Get_String
 *
 * @brief Reads expression
 *
 * @param[out]  instream    input file descriptor
 * @param[out]  str         pointer to string
 *
 * @retval  0
******************************************************************************/
int Get_String (FILE* instream, char* str);

/** ***************************************************************************
 * Get_G0
 *
 * @brief Gets G0 expression
 * Grammar G0:
 * G0 ::= N'\0'
 * N  ::= [0..9]
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
int Get_G0 (char* str);
int Get_N_G0();

/** ***************************************************************************
 * Get_G1
 *
 * @brief Gets G1 expression
 * Grammar G1:
 * G0 ::= N'\0'
 * N  ::= [0..9]+
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
int Get_G1 (char* str);
int Get_N_G1();

/** ***************************************************************************
 * Get_G2
 *
 * @brief Gets G2 expression
 * Grammar G2:
 * G2 ::= E'\0'
 * E  ::= N{[+-]N}*
 * N  ::= [0..9]+
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
int Get_G2 (char* str);
int Get_E_G2();
int Get_N_G2();

/** ***************************************************************************
 * Get_G3
 *
 * @brief Gets G3 expression
 * Grammar G3:
 * G3 ::= E'\0'
 * E  ::= T{[+-]T}*
 * T  ::= N{[* or /]N}*
 * N  ::= [0..9]+
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
double Get_G3 (char* str);
double Get_E_G3();
double Get_T_G3();
double Get_N_G3();

/** ***************************************************************************
 * Get_G4
 *
 * @brief Gets G4 expression
 * Grammar G4:
 * G4 ::= E'\0'
 * E  ::= T{[+-]T}*
 * T  ::= P{[* or /]P}*
 * P  ::= (E) or N
 * N  ::= [0..9]+
 *
 * @param[out]  str     pointer to expression
 *
 * @retval Result value.
******************************************************************************/
double Get_G4 (char* str);
double Get_E_G4();
double Get_T_G4();
double Get_P_G4();
double Get_N_G4();
