/**
 * @file stack.h
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Header file for stack.
 */

#ifndef IFJ_2020_STACK_H
#define IFJ_2020_STACK_H

#include "symtable.h"

typedef struct scopeItem{
    tHashTable* table;
    int scopeLevel;
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

#define MAX_STACK 100//DANGER!!!

typedef struct t_INT_Stack {
    int data[MAX_STACK];
    int top;
} t_INT_Stack;

void init_INT_Stack(t_INT_Stack* stack);
int add_INT_StackItem(t_INT_Stack* stack, int data);
int destroy_INT_Stack(t_INT_Stack* stack);
bool is_INT_Stack_Empty(t_INT_Stack* stack);
int top_INT_Stack(t_INT_Stack* stack);
int pop_INT_Stack(t_INT_Stack* stack);

#endif //IFJ_2020_STACK_H
