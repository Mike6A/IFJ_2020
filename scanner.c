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

int initTokenizer(tTokenizer* tokenizer) {
    tokenizer->eolFlag = EOL_OPTIONAL;
    tokenizer->errorCode = 0;
    tokenizer->isEOF = false;
    
    tToken token = {.value = "", .type = NONE};
    tokenizer->outputToken = token;

    tStringBuilder sb;
    if (initStringBuilder(&sb) == 1)
        return 1;
    tokenizer->sb = sb; 
    return 0;
}

void getNextChar(tTokenizer* tokenizer) {
    int _char = fgetc(stdin);
    if (_char == EOF){
        if (feof(stdin)){
            tokenizer->actualChar = 1;
            tokenizer->isEOF = true;
            return;
        } else {
            tokenizer->errorCode = 99;
            return;
        }
    } 
    tokenizer->actualChar = _char;
}

/**
 * @brief Check if actualChar is letter or underscore.
 * 
 * @return true is a letter
 * @return false is not a letter
 */
bool isActLetter(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return ((actualChar >= 'a' && actualChar <= 'z') || (actualChar >= 'A' && actualChar <= 'Z') || actualChar == '_') ? true : false;
}

/**
 * @brief  Check if actualChar is number;
 * 
 * @return true is number
 * @return false in not number
 */
bool isActNumber(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return (actualChar >= '0' && actualChar <= '9') ? true : false;
}

bool isSeparator(tTokenizer* tokenizer){
    for (int i = 0; i < sizeof(SEPARATORS) / sizeof(SEPARATORS[0]); i++)
        if (tokenizer->actualChar == SEPARATORS[i]) 
            return true;
    return false;
}

void state_EOF(tTokenizer* tokenizer){
    tokenizer->outputToken.type = token_EOF;
}

/**
 * @brief Get next token from input.
 * 
 * @param eolFlag Set how EOL should be handled
 * @param errCode Pointer to error code. Sets when an error occurs
 * @return tToken Return new token
 */
void getToken(tTokenizer* tokenizer) {
    if (tokenizer->processed) 
        getNextChar(tokenizer);

    if (tokenizer->isEOF){
        state_EOF(tokenizer);
        return;
    }

    if (tokenizer->eolFlag == EOL_REQUIRED){
        state_EOLRequired(tokenizer);
        return;
    }

    while (isSeparator(tokenizer)){
        getNextChar(tokenizer);
    }

    if (isActLetter(tokenizer)){
        state_ID(tokenizer);
        return;
    }

    if (isActNumber(tokenizer)){
        state_Num(tokenizer);
        return;
    }

    if (tokenizer->actualChar == '\n'){
        state_EOL(tokenizer);
        return;
    }
}

void state_EOLRequired(tTokenizer* tokenizer) {
    //check if is eol required
    if (tokenizer->eolFlag == EOL_REQUIRED){
        while (isSeparator(tokenizer)){
            getNextChar(tokenizer);
        }
        if (tokenizer->actualChar == '\n') {
            tokenizer->outputToken.value = "\n";
            tokenizer->outputToken.type = NONE;
        } else {
            tokenizer->errorCode = 1;
        }
    }
}

void state_ID(tTokenizer* tokenizer) {
    cleanBuilder(&tokenizer->sb);
    do {
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
  		getNextChar(tokenizer);
    } while (isActLetter(tokenizer) || isActNumber(tokenizer));
    tokenizer->processed = false;

	if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		tokenizer->errorCode = 99;
        return;
	}

    //check if is KEYWORD 
    for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); i++) {
        if (strcmp(tokenizer->outputToken.value, KEYWORDS[i]) == 0){
			tokenizer->outputToken.type = KW;
            return;
		}
    }

	tokenizer->outputToken.type = ID;
}

void state_Num(tTokenizer* tokenizer){
    cleanBuilder(&tokenizer->sb);
    do {
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
  		getNextChar(tokenizer);
    } while (isActNumber(tokenizer));

    //num with decimal part
    if (tokenizer->actualChar == '.'){
        state_BasicDouble(tokenizer);
        return;
    }

    //exp num without decimal part
    if (tokenizer->actualChar == 'e' || tokenizer->actualChar == 'E'){
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
        state_ExpNum(tokenizer);
        return;
    }

    //return token with INT
	if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		tokenizer->errorCode = 99;
        return;
	}
    tokenizer->outputToken.type = INT;
    tokenizer->processed = false;
}

void state_BasicDouble(tTokenizer* tokenizer){
    //loop until stdin is a digit
    do {
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
  		getNextChar(tokenizer);
    } while (isActNumber(tokenizer));

    //exp num with decimal part
    if (tokenizer->actualChar == 'e' || tokenizer->actualChar == 'E'){
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
        state_ExpNum(tokenizer);
        return;
    }

    //return token with double
	if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		tokenizer->errorCode = 99;
        return;
	}
    tokenizer->outputToken.type = DOUBLE;
    tokenizer->processed = false;
}

void state_ExpNum(tTokenizer* tokenizer){
  	getNextChar(tokenizer);
    if (isActNumber(tokenizer) || tokenizer->actualChar == '+' || tokenizer->actualChar == '-'){
        //loop until stdin is a digit
        do {
		    if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			    tokenizer->errorCode = 99;
                return;
		    }
  		    getNextChar(tokenizer);
        } while (isActNumber(tokenizer));

        //return token with double 
	    if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		    tokenizer->errorCode = 99;
            return;
	    }
        tokenizer->outputToken.type = DOUBLE;
    } else {
        tokenizer->errorCode = 1;
    }
    tokenizer->processed = false;
}

void state_EOL(tTokenizer* tokenizer){
    if (tokenizer->eolFlag == EOL_FORBIDEN){
        tokenizer->errorCode = 1;
    } else {
        tokenizer->outputToken.value = "";
        tokenizer->outputToken.type = token_EOL;
    }
    getNextChar(tokenizer);
    tokenizer->processed = false;
}
