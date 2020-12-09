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
 * @brief This function delete function parameters
 * 
 * @param func pointer to function item
 */
void deleteFunctItem(tFuncItem *func){
    for(unsigned i = 0; i < func->params_count; i++)
        free(func->params[i]);
}

/**
 * @brief This function delete item.
 * 
 * @param item Valid pointer to Hash table item.
 */
void deleteItem(tHashItem* item){
    free(item->id);
    if (item->value != NULL)
        free(item->value);
    if (item->func != NULL){
        deleteFunctItem(item->func);
        free(item->func);
    }
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
 * @brief This function get item from HT.
 * 
 * @param ht valid pointer to hash table.
 * @param id id of item 
 * @return pointer to HashItem if was found \n
 *         NULL if wasnot found
 */
tHashItem* getHashItem(tHashTable* ht, char* id){
    int key = getHash(ht, id);
    for(tHashItem* tmp = ht->table[key]; tmp != NULL; tmp = tmp->next)
        if (strcmp(tmp->id, id) == 0)
            return tmp;
    return NULL;
}

/**
 * @brief This function add item to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param item Item which will be added to Hash table.
 * @return 0 if is correctly added \n
 *         1 if is already in HT \n
 *         2 if is already in HT but type is different
 */
int addItemToHT(tHashTable* ht, tHashItem* item) {
    tHashItem* searched = getHashItem(ht, item->id);
    if (searched == NULL) {
        int key = getHash(ht, item->id);
        if (ht->table[key] == NULL){
            ht->table[key] = item;
            return 0;
        }
        item->next = ht->table[key];
        ht->table[key] = item;
        return 0;
    } else {
        if (searched->type == item->type)
            return 1;
        return 2;
    }
}

/**
 * @brief This function add variable to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param id Variable ID.
 * @param value Value of variable.
 * @param declared if is variable declared.
 * @return 0 if is correctly added \n
 *         1 if is already in HT \n
 *         2 if is already in HT but type is different \n
 *         99 if malloc wasn't successful
 */
int addVarToHT(tHashTable* ht, char* id, TItem type, char* value, bool declared) {
    tHashItem* item = (tHashItem*)malloc(sizeof(tHashItem));
    item->type = type;

    item->id = malloc(sizeof(char) * (strlen(id) + 1)); //id
    if (item->id == NULL)
        return 99;
    strcpy(item->id, id);

    item->value = malloc(sizeof(char) * (strlen(value) + 1)); //value
    if (item->value == NULL)
        return 99;
    strcpy(item->value, value);

    item->declared = declared;
    item->next = NULL;
    item->func = NULL;

    return addItemToHT(ht, item);
}

/**
 * @brief This funtion add function to Hash table.
 * 
 * @param ht Valid pointer to hash table.
 * @param id Function ID.
 * @param declared if is function declared.
 * @return 0 if is correctly added \n
 *         1 if is already in HT \n
 *         2 if is already in HT but type is different \n
 *         99 if malloc wasn't successful
 */
int addFuncToHT(tHashTable* ht, char* id, bool declared) {
    tHashItem* item = (tHashItem*)malloc(sizeof(tHashItem));

    item->id = (char*)malloc(sizeof(char) * (strlen(id) + 1)); //id
    if (item->id == NULL)
        return 99;
    strcpy(item->id, id);

    item->value = NULL;
    item->declared = declared;
    item->type = TFunc;
    item->next = NULL;

    item->func = (tFuncItem*)malloc(sizeof(tFuncItem));
    if (item->func == NULL)
        return 99;
    item->func->params_count = 0;
    item->func->return_count = 0;

    return addItemToHT(ht, item);
}

/**
 * @brief This function add params to function
 * 
 * @param ht Valid pointer to hash table.
 * @param id Function ID.
 * @param param Id of param.
 * @param type Type of param.
 * @return 0 if is correctly added \n
 *         1 if ID wasn't found\n
 *         99 if malloc wasn't successful
 */
int addParamToFunc(tHashTable* ht, char* id, char* param, TItem type){
    tHashItem* searched = getHashItem(ht, id);
    if (searched != NULL && searched->type == TFunc) {
        unsigned index = searched->func->params_count++;
        searched->func->params[index] = malloc(sizeof(char) * (strlen(param) + 1)); //value
        if (searched->func->params[index] == NULL)
            return 99;
        strcpy(searched->func->params[index], param);
        searched->func->paramsTypes[index] = type;
        return 0;
    }
    return 1;
}

/**
 * @brief This function add return type to function
 * 
 * @param ht Valid pointer to hash table.
 * @param id Function ID.
 * @param type Type of return value.
 * @return 0 if is correctly added \n
 *         1 if ID wasn't found \n
 *         99 if malloc wasn't successful
 */
int addReturnTypeToFunc(tHashTable* ht, char* id, TItem type){
    tHashItem* searched = getHashItem(ht, id);
    if (searched != NULL && searched->type == TFunc) {
        unsigned index = searched->func->return_count++;
        searched->func->return_vals[index] = type;
        return 0;
    }
    return 1;
}

/**
 * @brief This function remove item from HT. 
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
            return 0;
        }
        while(tmp->next != NULL) {
            if (strcmp(tmp->next->id, id) == 0) {
                //delete + corr ptr
                tHashItem* toDelete = tmp->next;
                tmp->next = tmp->next->next;
                deleteItem(toDelete);
                return 0;
            }
            tmp = tmp->next;
        }
    }
    return 1;
}

/**
 * @brief This funtion check if is declared func/var.
 * 
 * @param ht Valid pointer to hash table.
 * @param id String ID.
 * @return true/false if item was found
 * @return false if item wasn't found
 */
bool isDeclared(tHashTable* ht, char *id){
    tHashItem* searched = getHashItem(ht, id);
    if (searched != NULL)
        return searched->declared;
    return false;
}