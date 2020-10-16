#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"

typedef enum { KW, INT, DOUBLE, STRING, NONE } TokenType;
typedef enum { EOL_REQUIRED, EOL_OPTIONAL, EOL_FORBIDEN } EOLflag;

char RUNE_LITERALS[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\''};

typedef struct {
    TokenType type;
    char* value;
} tToken;
