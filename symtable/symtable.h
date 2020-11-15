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
#define MAX_PARAMS_COUNT 100

typedef enum { TInt, TDouble, TString, TBool, TFunc } TItem;

typedef struct t_Func {
    char* params[MAX_PARAMS_COUNT]; /**< function parameters */
    TItem paramsTypes[MAX_PARAMS_COUNT]; /**< parameters types */
    TItem return_vals[MAX_PARAMS_COUNT]; /**< return types */
    unsigned return_count; /**< count of return types */
    unsigned params_count; /**< count of function parameters */
} tFuncItem;

/**
 * @brief Hash table item structure
 */
typedef struct t_HashItem {
    TItem type; /**< type of item */
    char* id; /**< ID */
    char* value; /**< value */
    bool declared; /**< if is declared */
    tFuncItem* func; /**< pointer to Func parametrs */
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

int addVarToHT(tHashTable* ht, char* id, TItem type, char* value, bool declared);
int addFuncToHT(tHashTable* ht, char* id, bool declared);
int addParamToFunc(tHashTable* ht, char* id, char* param, TItem type);
int addReturnTypeToFunc(tHashTable* ht, char* id, TItem type);
int removeHashItem(tHashTable* ht, char* id);
tHashItem* getHashItem(tHashTable* ht, char* id);
bool isDeclared(tHashTable* ht, char *id);


#endif