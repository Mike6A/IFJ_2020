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
#include "scanner.h"

char actualChar;
bool processed = true;
char *actualString = "";

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
bool isActLetter() {
    return ((actualChar > 'a' && actualChar < 'z') || (actualChar > 'A' && actualChar < 'Z') || actualChar == '_') ? true : false;
}

/**
 * @brief  Check if actualChar is number;
 * 
 * @return true is number
 * @return false in not number
 */
bool isActNumber() {
    return (actualChar > '0' && actualChar < '9') ? true : false;
}

/**
 * @brief Check if char array contains actualChar.
 * 
 * @param arr Char array
 * @return true if is in array
 * @return false if is not in array 
 */
bool is(char arr[]){
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        if (actualChar == arr[i]) 
            return true;
    }
    return false;
}

/**
 * @brief Get next token from input.
 * 
 * @param eolFlag Set how EOL should be handled
 * @param errCode Pointer to error code. Sets when an error occurs
 * @return tToken Return new token
 */
tToken getToken(EOLflag eolFlag, int* errCode) {
    if (processed) {
        getNextChar();
    }

    if (eolFlag == EOL_REQUIRED){
        return state_EOLRequired(eolFlag, errCode);
    }

    if (isActLetter()){
        return state_ID(eolFlag, errCode);
    }
}

tToken state_EOLRequired(EOLflag eolFlag, int* errCode) {
    tToken token = {.value = "", .type = NONE};
    //check if is eol required
    if (eolFlag == EOL_REQUIRED){
        while (is(SEPARATORS)){
            getNextChar();
        }
        if (actualChar == '\n') {
            token.value = "\n";
        } else {
            *errCode = 1;
        }
        return token;
    }
}

tToken state_ID(EOLflag eolFlag, int* errCode) {
	tStringBuilder sb = {.value = "", .len = 0, .allocated = 0 };
    do {
		appendChar(&sb, actualChar);
  		getNextChar();
    } while (isActLetter());
    processed = false;

	char* word = getStringFromBuilder(&sb);
	destructBuilder(&sb);
    tToken token = {.value = word, .type = NONE};
    //check if is KEYWORD 
    for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); i++) {
        if (strcmp(word, KEYWORDS[i])){
			token.type = KW;
			return token;
		}
    }

	token.type = ID;
    return token;
}