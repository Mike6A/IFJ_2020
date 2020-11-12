/**
 * @file semantics.h
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Header file for semantics.
 */

#include "symtable/symtable.h"
#include "stack.h"
#include "analyse.h"

#ifndef IFJ_2020_SEMANTICS_H
#define IFJ_2020_SEMANTICS_H

long runSemanticAnalyze(SyntaxNode* root, tScope* scope);

#endif //IFJ_2020_SEMANTICS_H
