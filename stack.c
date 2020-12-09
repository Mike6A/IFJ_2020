/**
 * @file stack.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief File for stack
 */

#include "stack.h"

void createList(tStringLinkedListItem* list) {
    list->next = NULL;
    list->value = NULL;
}

/**
 * Adds string to the list. Use only malloc-ed strings...
 * @param list Pointer to list
 * @param str ONLY FROM MALLOC!!!! OR WILL CRASH PROGRAM
 * @return 99 if malloc error
 */
int addListItem(tStringLinkedListItem* list, char* str) {
    if (list->value == NULL) {
        list->value =  str;
        return 0;
    }

    tStringLinkedListItem* node = malloc(sizeof(tStringLinkedListItem));
    if (node == NULL) {
        return 99;
    }

    node->value = str;
    node->next = NULL;

    tStringLinkedListItem* tmp = list;
    while (tmp != NULL && tmp->next != NULL) {
        tmp = tmp->next;
    }

    tmp->next = node;
    return 0;
}

int destroyList(tStringLinkedListItem* list) {
    while (list != NULL) {
        tStringLinkedListItem* tmp = list;
        list = list->next;
        if (tmp->value != NULL) {
            free(tmp->value);
        }
        free(tmp);
    }
    return 0;
}


void initScope(tScope* scope) {
    scope->topLocal = NULL;
    scope->global = NULL;
}

int createScope(tScope *scope) {
    tHashTable* table = (tHashTable*) malloc(sizeof(tHashTable));
    initHashTable(table, 5);    //TODO 1009
    if (table == NULL) {
        return 99;
    }

    if (scope->topLocal == NULL) {
        tScopeItem* newScope = (tScopeItem*) malloc(sizeof(tScopeItem));
        newScope->next = NULL;
        newScope->scopeLevel = 0;
        scope->topLocal = newScope;
        scope->topLocal->table = table;
        scope->global = newScope;
    }
    else {
        tScopeItem* newScope = (tScopeItem*) malloc(sizeof(tScopeItem));
        if (newScope == NULL) {
            free(table);
            return 99;
        }

        int scopeLevel = 0;
        tScopeItem* tmp = scope->topLocal;
        while (tmp != NULL) {
            tmp = tmp->next;
            scopeLevel++;
        }
        newScope->scopeLevel = scopeLevel;

        newScope->next = scope->topLocal;
        newScope->table = table;
        scope->topLocal = newScope;
    }
    return 0;
}

int removeLastLocalScope(tScope* scope){
    tScopeItem *tmp = scope->topLocal;
    scope->topLocal = tmp->next;
    destructHashTable(tmp->table);
    free(tmp->table);
    free(tmp);
    return 0;
}

void init_INT_Stack(t_INT_Stack* stack){
    for(int i = 0; i < MAX_STACK; ++i)
        stack->data[i] = -1;
    stack->top = 0;
}
int add_INT_StackItem(t_INT_Stack* stack, int data){
    if(stack->top < MAX_STACK) {
        stack->data[++stack->top] = data;
        return 0;
    }
    return -1;
}
int destroy_INT_Stack(t_INT_Stack* stack){
    stack->top = 0;
    return 0;
}
bool is_INT_Stack_Empty(t_INT_Stack* stack){
    if (stack->top == 0)
        return true;
    return false;
}
int top_INT_Stack(t_INT_Stack* stack){
    if (stack->top < MAX_STACK && stack->top >= 0)
        return stack->data[stack->top];
    return -1;
}
int pop_INT_Stack(t_INT_Stack* stack){
    if (stack->top < MAX_STACK && stack->top >= 0)
        return stack->data[stack->top--];
    return -1;
}

