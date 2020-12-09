/**
 * @file scanner.h
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Header file for lexical analysis.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "stringBuilder.h"

typedef enum { tokenType_ID, tokenType_KW, tokenType_INT, tokenType_DOUBLE, tokenType_STRING, tokenType_LE, 
               tokenType_GE, tokenType_EQ, tokenType_NEQ, tokenType_DECL, tokenType_ASSMUL, tokenType_ASSPLUS, tokenType_ASSMINUS,
    tokenType_NONE, tokenType_EOF, tokenType_PLUS, tokenType_MINUS, tokenType_MUL, tokenType_DIV, tokenType_LESS, tokenType_GREATER,
               tokenType_LBN, tokenType_RBN, tokenType_LBC, tokenType_RBC,
               tokenType_COMMA, tokenType_SCOMMA, tokenType_ASSIGN, tokenType_EOL, tokenType_ASSDIV} TokenType;


/**
 * @brief Token structure
 */
typedef struct {
    TokenType type; /**< type of token */
    char* value; /**< content of token (string). if len < 2 is not allocated */
} tToken;

/**
 * @brief Tokenizer structure
 */
typedef struct {
    tStringBuilder sb; /**< string builder */
    tToken outputToken; /**< output token */
    int errorCode; /**< error code */
    char actualChar; /**< actual loaded char from stdin */
    bool processed; /**< if is actualChar is recognized */
    bool isEOF; /**< if is EOF */
} tTokenizer;

int initTokenizer(tTokenizer* tokenizer);
void destructTokenzier(tTokenizer* tokenizer);

void getNextChar(tTokenizer* tokenizer);
bool isActLetter(tTokenizer* tokenizer);
bool isActNumber(tTokenizer* tokenizer);
bool isSeparator(tTokenizer* tokenizer);

void getToken(tTokenizer* tokenizer);
void state_EOF(tTokenizer* tokenizer);
int state_EOL(tTokenizer* tokenizer);

void state_ID(tTokenizer* tokenizer);
void state_String(tTokenizer* tokenizer);

void state_Num(tTokenizer* tokenizer);
void state_BasicDouble(tTokenizer* tokenizer);
void state_ExpNum(tTokenizer* tokenizer);

int state_OneLineComment(tTokenizer* tokenizer);
int state_BlockComment(tTokenizer* tokenizer);

int state_SecondEq(tTokenizer* tokenizer);
void freeToken(tTokenizer* tokenizer);
#endif