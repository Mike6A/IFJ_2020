/**
 * @file scenner.c
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Implementaion of lexical analysis for IFJ20.
 * @version 0.1
 * @date 2020-10-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "scenner.h"

char actualChar;
char *actualString;

/**
 * @brief Get the next char from stdin and put into actualChar variable.
 */
void getNextChar() {
    actualChar = getc(stdin);
}

/**
 * @brief Check if actualChar is letter or underscore.
 * 
 * @return true is a letter
 * @return false is not a letter
 */
bool isActLetter(){
    return ((actualChar > 'a' && actualChar < 'z') || (actualChar > 'A' && actualChar < 'Z') || actualChar == '_') ? true : false;
}

/**
 * @brief  Check if actualChar is number;
 * 
 * @return true is number
 * @return false in not number
 */
bool isActNumber(){
    return (actualChar > '0' && actualChar < '9') ? true : false;
}

/**
 * @brief Get next token from input.
 * 
 * @param eolFlag Set how EOL should be handled
 * @param errCode Pointer to error code. Sets when an error occurs
 * @return tToken Return new token
 */
tToken getToken(EOLflag eolFlag, int* errCode) {
    getNextChar();
}