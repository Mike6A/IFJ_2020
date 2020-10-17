#include "stringBuilder.h"

int initStringBuilder(tStringBuilder* builder) {
    builder->value = (char*)malloc(16*sizeof(char));
    if (builder->value == NULL){
        return 1;
    }
    builder->allocated = 16;
    builder->len = 0;
    return 0;
}

void destructBuilder(tStringBuilder* builder){
    free(builder->value);
}

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

void cleanBuilder(tStringBuilder* builder){
    if (builder->len > 0 ) {
        builder->value[0] = '\0'; 
        builder-> len = 0;
    }
}

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

int getStringAndCleanBuilder(tStringBuilder* builder, char** dest){
    if(getStringFromBuilder(builder, dest) == 1)
        return 1;
    cleanBuilder(builder);
    return 0;
}