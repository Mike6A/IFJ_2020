#include "stringBuilder.h"

int initStringBuilder(tStringBuilder* builder) {
    tStringBuilder* _builder = (tStringBuilder*)malloc(sizeof(tStringBuilder));
    if (_builder == NULL)
        return 1;
    _builder->value = (char*)malloc(sizeof(char) * 10);
    if (_builder->value == NULL) {
        free(_builder);
        return 1;
    }
    _builder->value[0] = '\0';
    _builder->allocated = 10;
    _builder->len = 0;
    builder = _builder;
    return 0;
}

void destructBuilder(tStringBuilder* builder){
    free(builder->value);
    free(builder);
}

int appendChar(tStringBuilder* builder, char c) {
    if (builder->allocated - 1 <= builder->len) {
        char* new_string;
        new_string = (char*)malloc(builder->allocated + 10*sizeof(char));
        if (new_string != NULL) {
            for (int i = 0; i < builder->len; i++)
                new_string[i] = builder->value[i];
            if(builder->len > 0)
                free(builder->value);
            builder->value = new_string;
            builder->value[builder->len] = c;
            builder->value[builder->len + 1] = '\0';
            builder->allocated += 10*sizeof(char);
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
    }
}

char* getStringFromBuilder(tStringBuilder* builder) {
    if (builder->len == 0 ) 
        return "";
    char* _new;
    _new = (char*)malloc(builder->len + 1);
    for (int i = 0; i < builder->len + 1; i++)
        _new[i] = builder->value[i];
    return _new;
}

char* getStringAndCleanBuilder(tStringBuilder* builder){
    char* value = getStringFromBuilder(builder);
    cleanBuilder(builder);
    return value;
}