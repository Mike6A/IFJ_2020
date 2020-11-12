/**
 * @file stack.h
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Header file for stack.
 */

#include "symtable/symtable.h"

#ifndef IFJ_2020_STACK_H
#define IFJ_2020_STACK_H

typedef struct scopeItem{
    tHashTable* table;
    struct scopeItem* next;
} tScopeItem;

typedef struct scope{
    tScopeItem* topLocal;
    tScopeItem* global;
}tScope;

void initScope(tScope* scope);
int createScope(tScope *scope);
int removeLastLocalScope(tScope *scope);

#endif //IFJ_2020_STACK_H
