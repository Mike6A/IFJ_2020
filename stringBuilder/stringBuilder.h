#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

typedef struct {
    char* value;
    unsigned len;
    unsigned allocated;
} tStringBuilder;

int initStringBuilder(tStringBuilder* builder);
void destructBuilder(tStringBuilder* builder);
int appendChar(tStringBuilder* builder, char c);
void cleanBuilder(tStringBuilder* builder);
int getStringFromBuilder(tStringBuilder* builder, char** dest);
int getStringAndCleanBuilder(tStringBuilder* builder, char** dest);