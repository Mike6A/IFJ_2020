/**
 * @file semantics.h
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Header file for semantics.
 */

#ifndef IFJ_2020_SEMANTICS_H
#define IFJ_2020_SEMANTICS_H

#include "symtable/symtable.h"
#include "stack.h"
#include "analyse.h"

typedef struct t_ExpReturnType {
    TItem type; /**< type of item */
    char* value; /**< value */
    long errCode; /**< return code  */
    bool constant;  /**< true if item is constant (known value) | false if unknown value */
} tExpReturnType;

long runSemanticAnalyze(SyntaxNode* root);

#endif //IFJ_2020_SEMANTICS_H
