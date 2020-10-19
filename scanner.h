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
#include "stringBuilder/stringBuilder.h"

typedef enum { t_ID, t_KW, t_INT, t_DOUBLE, t_STRING, t_NONE, t_EOF } TokenType;
typedef enum { EOL_REQUIRED, EOL_OPTIONAL, EOL_FORBIDEN } EOLflag;


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
    EOLflag eolFlag; /** EOL flag */
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
void state_EOLRequired(tTokenizer* tokenizer);
int state_EOL(tTokenizer* tokenizer);

void state_ID(tTokenizer* tokenizer);
void state_String(tTokenizer* tokenizer);

void state_Num(tTokenizer* tokenizer);
void state_BasicDouble(tTokenizer* tokenizer);
void state_ExpNum(tTokenizer* tokenizer);

int state_OneLineComment(tTokenizer* tokenizer);
int state_BlockComment(tTokenizer* tokenizer);

int state_SecondEq(tTokenizer* tokenizer);
#endif