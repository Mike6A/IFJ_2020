/**
 * @file symtable.h
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Implementaion of symbol table with hash table.
 */
#include "symtable.h"

/**
 * @brief This function get hash key from string.
 * 
 * @param ht Valid pointer to hash table.
 * @param value String.
 * @return int key
 */
int getHash(tHashTable* ht, char* value){
    int i;
    for (i = 0; i < strlen(value); i++)
        i += value[i];
    return i % ht->size;
}

/**
 * @brief This function initialize hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param size size of Hash table.
 * @return 0 if alloc was successful
 * @return 2 if size is bigger than max size of hash table 
 */
int initHashTable(tHashTable* ht, int size){
    if (size > MAX_HASHTABLE_SIZE)
        return 2;
    ht->size = size;

    ht->table = (tHashItem**)malloc(sizeof(tHashItem*) * size);
    if (ht->table == NULL)
        return 1;
    for (int i = 0; i < size; i++)
        ht->table[i] = NULL;
    return 0; 
}

/**
 * @brief This function delete item.
 * 
 * @param item Valid pointer to Hash table item.
 */
void deleteItem(tHashItem* item){
    free(item->id);
    if (strlen(item->value) > 0)
        free(item->value);
    if (item->params_count > 0)
        for (int j = 0; j < item->params_count; j++)
            free(item->params[j]);
    free(item);
}

/**
 * @brief This function destruct Hash table.
 * 
 * @param ht Valid pointer to hash table.
 */
void destructHashTable(tHashTable* ht) {
    for (int i = 0; i < ht->size; i++) {
        tHashItem* tmp = ht->table[i];
        while (tmp != NULL) {
            tHashItem* toDelete = tmp;
            tmp = tmp->next;
            deleteItem(toDelete);
        }
    }
    free(ht->table);
    ht->size = 0;
}

/**
 * @brief This function add item to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param item Item which will be added to Hash table.
 * @param key Hash key.
 */
void addItemToHT(tHashTable* ht, tHashItem* item, int key) {
    tHashItem* tmp = ht->table[key];
    if (tmp == NULL)
        ht->table[key] = item;
    else {
        while(tmp->next != NULL)
            tmp = tmp->next;
        
        tmp->next = item;
    }
}

/**
 * @brief This function add variable to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param id Variable ID.
 * @param value Value of variable.
 * @param declared if is variable declared.
 * @return 0 if alloc was successful 
 */
int addDataToHT(tHashTable* ht, char* id, char* value, bool declared) {
    tHashItem* item = (tHashItem*)malloc(sizeof(tHashItem));

    item->id = malloc(sizeof(char) * (strlen(id) + 1));
    if (item->id == NULL)
        return 1;
    strcpy(item->id, id);

    item->value = malloc(sizeof(char) * (strlen(value) + 1));
    if (item->value == NULL)
        return 1;
    strcpy(item->value, value);

    item->params_count = 0;
    item->declared = declared;
    item->type = TData;
    item->next = NULL;

    int key = getHash(ht, id);
    addItemToHT(ht, item, key);
    return 0;
}

/**
 * @brief This funtion add function to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param id Function ID.
 * @param params Function parameters.
 * @param params_count Count of function parameters.
 * @param declared if is function declared.
 * @return 0 if alloc was successful
 */
int addFuncToHT(tHashTable* ht, char* id, char** params, unsigned params_count, bool declared) {
    tHashItem* item = (tHashItem*)malloc(sizeof(tHashItem));

    item->id = malloc(sizeof(char) * (strlen(id) + 1));
    if (item->id == NULL)
        return 1;
    strcpy(item->id, id);

    if (params_count > 0) {
        item->params = malloc(sizeof(char*) * params_count);
        for(int i = 0; i < params_count; i++) {
            item->params[i] = malloc(sizeof(char) * (strlen(params[i]) + 1));
            if (item->params[i] == NULL)
                return 1;
        }
    }
    item->params_count = 0;
    item->declared = declared;
    item->type = TFunc;
    item->next = NULL;

    int key = getHash(ht, id);
    addItemToHT(ht, item, key);
    return 0;
}

/**
 * @brief 
 * 
 * @param ht Valid pointer to hash table.
 * @param id String ID.
 * @return 1 if item wasn't found 
 */
int removeHashItem(tHashTable* ht, char* id) {
    int key = getHash(ht, id);
    tHashItem* tmp = ht->table[key];
    if (tmp != NULL) {
        if (tmp->next == NULL || strcmp(tmp->id, id) == 0) { //first
            //delete + corr ptr
            ht->table[key] = tmp->next;
            deleteItem(tmp);
            return 1;
        }
        while(tmp->next != NULL) {
            if (strcmp(tmp->next->id, id) == 0) {
                //delete + corr ptr
                tHashItem* toDelete = tmp->next;
                tmp->next = tmp->next->next;
                deleteItem(toDelete);
                return 1;
            }
            tmp = tmp->next;
        }
    }
    return 1;
}