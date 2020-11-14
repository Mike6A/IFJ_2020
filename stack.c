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
    if (list->value != NULL) {
        free(list->value);
    }
    list = list->next;
    while (list != NULL) {
        tStringLinkedListItem* tmp = list;
        list = list->next;
        free(tmp->value);
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