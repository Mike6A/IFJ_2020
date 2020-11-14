/**
 * @file stack.h
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Header file for stack.
 */

#ifndef IFJ_2020_STACK_H
#define IFJ_2020_STACK_H

#include "symtable/symtable.h"

typedef struct scopeItem{
    tHashTable* table;
    struct scopeItem* next;
} tScopeItem;

typedef struct scope{
    tScopeItem* topLocal;
    tScopeItem* global;
} tScope;

void initScope(tScope* scope);
int createScope(tScope *scope);
int removeLastLocalScope(tScope *scope);


typedef struct t_StringLinkedListItem {
    char* value;
    struct t_StringLinkedListItem* next;
} tStringLinkedListItem;

void createList(tStringLinkedListItem* list);
int addListItem(tStringLinkedListItem* list, char* str);
int destroyList(tStringLinkedListItem* list);

#endif //IFJ_2020_STACK_H
