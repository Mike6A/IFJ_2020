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
