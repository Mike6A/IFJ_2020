#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

typedef enum { KW, INT, DOUBLE, STRING, NONE } TokenType;

char RUNE_LITERALS[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\''};

typedef struct {
    TokenType type;
    char* value;
} tToken;
