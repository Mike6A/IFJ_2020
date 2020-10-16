#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

typedef struct {
    char* value;
    unsigned len;
    unsigned allocated;
} tStringBuilder;

void initStringBuilder(tStringBuilder* builder);
void destructBuilder(tStringBuilder* builder);
int appendChar(tStringBuilder* builder, char c);
void cleanBuilder(tStringBuilder* builder);
char* getStringFromBuilder(tStringBuilder* builder);
char* getStringAndCleanBuilder(tStringBuilder* builder);