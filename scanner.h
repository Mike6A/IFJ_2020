#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

typedef enum { KW, INT, DOUBLE, STRING, NONE } TokenType;
typedef enum { EOL_REQUIRED, EOL_OPTIONAL, EOL_FORBIDEN } EOLflag;

char RUNE_LITERALS[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\''};
char SEPARATORS[] = {' ', '\t'};
char* KEYWORDS[] = {"if", "else", "float64", "for", "func", "int", "package", "return", "string"};

typedef struct {
    TokenType type;
    char* value;
} tToken;

void getNextChar();
bool isActLetter();
bool isActNumber();
bool is(char arr[]);

bool appendChar();
void clearString();
char* copyString();

tToken getToken(EOLflag eolFlag, int* errCode);
tToken state_EOLRequired(EOLflag eolFlag, int* errCode);
tToken state_ID(EOLflag eolFlag, int* errCode);