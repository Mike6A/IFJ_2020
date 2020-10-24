/**
 * @file symtable.h
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Header file for symtable.
 */
#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_HASHTABLE_SIZE 65536

typedef enum { TData, TFunc } TItem;

/**
 * @brief Hash table item structure
 */
typedef struct t_HashItem {
    TItem type; /**< type of item */
    char* id; /**< ID */
    char* value; /**< value */
    bool declared; /**< if is declared */
    char** params; /**< function parameters */
    unsigned params_count; /**< count of function parameters */
    struct t_HashItem* next; /**< pointer to next item with same key */
} tHashItem;

/**
 * @brief  Hash table struct
 */
typedef struct t_HashTable {
    tHashItem** table; /**< table with pointers to items */
    int size; /**< size of hash table */
} tHashTable;

int initHashTable(tHashTable* ht, int size);
void destructHashTable(tHashTable* ht);
int addDataToHT(tHashTable* ht, char* id, char* value, bool declared);
int addFuncToHT(tHashTable* ht, char* id, char** params, unsigned params_count, bool declared);
int removeHashItem(tHashTable* ht, char* id);
tHashItem* getHashItem(tHashTable* ht, char* id);


#endif