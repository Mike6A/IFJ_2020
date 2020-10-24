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
