/**
 * @file stringBuilder.c
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Implementaion of string builder for IFJ20.
 */
#include "stringBuilder.h"

/**
 * @brief This function init string builder.
 * 
 * @param builder valid pointer to StringBuilder struct.
 * @return 0 if alloc was successful
 */
int initStringBuilder(tStringBuilder* builder) {
    builder->value = (char*)malloc(16*sizeof(char));
    if (builder->value == NULL){
        return 1;
    }
    builder->allocated = 16;
    builder->len = 0;
    return 0;
}

/**
 * @brief This function destruct string builder.
 * 
 * @param builder valid pointer to StringBuilder struct.
 */
void destructBuilder(tStringBuilder* builder){
    free(builder->value);
}

/**
 * @brief This function append char to string in String builder.
 * 
 * @param builder valid pointer to StringBuilder struct.
 * @param c char which will be append to string.
 * @return 0 if alloc was successful
 */
int appendChar(tStringBuilder* builder, char c) {
    if ((int)builder->allocated - 1 <= (int)builder->len) {
        char* new_string;
        new_string = (char*)malloc(builder->allocated*2);
        if (new_string != NULL) {
            for (int i = 0; i < builder->len; i++)
                new_string[i] = builder->value[i];
            if(builder->len > 0)
                free(builder->value);
            builder->value = new_string;
            builder->value[builder->len] = c;
            builder->value[builder->len + 1] = '\0';
            builder->allocated *= 2;
            builder->len++;
        }
        else {
            free(builder->value);
            return 1;
        }
    }
    else {
        builder->value[builder->len] = c;
        builder->value[builder->len + 1] = '\0';
        builder->len++;
    }
    return 0;
}

/**
 * @brief This function sets String builder value(string) to "".
 *        Not deallocate memory.
 * 
 * @param builder valid pointer to StringBuilder struct.
 */
void cleanBuilder(tStringBuilder* builder){
    if (builder->value != NULL && builder->len > 0 ) {
        builder->value[0] = '\0'; 
        builder-> len = 0;
    }
}

/**
 * @brief This function get string from String builder to destination.
 * 
 * @param builder valid pointer to StringBuilder struct.
 * @param dest string destination, get new pointer to string. (copy)
 * @return 0 if alloc was successful
 */
int getStringFromBuilder(tStringBuilder* builder, char** dest) {
    if (builder->len == 0 ) 
        *dest = "";
    char* _new;
    _new = (char*)malloc(builder->len + 1);
    if (_new == NULL)
        return 1;
    for (int i = 0; i < builder->len + 1; i++)
        _new[i] = builder->value[i];
    *dest = _new;
    return 0;
}

/**
 * @brief This function sets String builder value(string) to "" and get string from String builder to destination.
 *        Not deallocate memory.
 * 
 * @param builder valid pointer to StringBuilder struct.
 * @param dest string destination, get new pointer to string. (copy)
 * @return 0 if alloc was successful
 */
int getStringAndCleanBuilder(tStringBuilder* builder, char** dest){
    if(getStringFromBuilder(builder, dest) == 1)
        return 1;
    cleanBuilder(builder);
    return 0;
}