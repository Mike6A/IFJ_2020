/**
 * @file stack.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief File for stack
 */

#include "stack.h"

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