#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stringBuilder/stringBuilder.h"

typedef enum { ID, KW, INT, DOUBLE, STRING, NONE, token_EOF, token_EOL } TokenType;
typedef enum { EOL_REQUIRED, EOL_OPTIONAL, EOL_FORBIDEN } EOLflag;

char RUNE_LITERALS[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\''};
char SEPARATORS[] = {' ', '\t'};
char* KEYWORDS[] = {"if", "else", "float64", "for", "func", "int", "package", "return", "string"};

typedef struct {
    TokenType type;
    char* value;
} tToken;

typedef struct {
    tStringBuilder sb;
    tToken outputToken;
    int errorCode;
    EOLflag eolFlag;
    char actualChar;
    bool processed;
    bool isEOF;
} tTokenizer;

int initTokenizer(tTokenizer* tokenizer);

void getNextChar(tTokenizer* tokenizer);
bool isActLetter(tTokenizer* tokenizer);
bool isActNumber(tTokenizer* tokenizer);
bool isSeparator(tTokenizer* tokenizer);

void getToken(tTokenizer* tokenizer);
void state_EOF(tTokenizer* tokenizer);
void state_EOLRequired(tTokenizer* tokenizer);
void state_EOL(tTokenizer* tokenizer);

void state_ID(tTokenizer* tokenizer);

void state_Num(tTokenizer* tokenizer);
void state_BasicDouble(tTokenizer* tokenizer);
void state_ExpNum(tTokenizer* tokenizer);