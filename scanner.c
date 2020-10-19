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
    
    tToken token = {.value = "", .type = t_NONE};
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
bool isActHexNum(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return ((actualChar >= '0' && actualChar <= '9') ||
     (actualChar >= 'a' && actualChar <= 'f') ||
     (actualChar >= 'A' && actualChar <= 'F'));
}

bool isSeparator(tTokenizer* tokenizer){
    for (int i = 0; i < sizeof(SEPARATORS) / sizeof(SEPARATORS[0]); i++)
        if (tokenizer->actualChar == SEPARATORS[i]) 
            return true;
    return false;
}

bool isRuneLitaral(tTokenizer* tokenizer){
    for (int i = 0; i < sizeof(RUNE_LITERALS) / sizeof(RUNE_LITERALS[0]); i++)
        if (tokenizer->actualChar == RUNE_LITERALS[i]) 
            return true;
    return false;
}

void state_EOF(tTokenizer* tokenizer){
    tokenizer->outputToken.type = t_EOF;
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

    if (tokenizer->eolFlag == EOL_REQUIRED){
        state_EOLRequired(tokenizer);
        return;
    }

    while (isSeparator(tokenizer)){
        getNextChar(tokenizer);
    }

    if(tokenizer->actualChar == '/'){
        if (state_OneLineComment(tokenizer) != 0){
            return;
        }
    }

    while (isSeparator(tokenizer)){
        getNextChar(tokenizer);
    }

    if (tokenizer->isEOF){
        state_EOF(tokenizer);
        return;
    }
    
    if (isActLetter(tokenizer)){
        state_ID(tokenizer);
        return;
    }

    if (isActNumber(tokenizer)){
        state_Num(tokenizer);
        return;
    }

    if(tokenizer->actualChar == '"'){
        state_String(tokenizer);
        return;
    }

    tokenizer->outputToken.type = t_NONE;
    tokenizer->processed = true;
    switch (tokenizer->actualChar) {
        case '+':
            tokenizer->outputToken.value = "+";
            return;
        case '-':
            tokenizer->outputToken.value = "-";
            return;
        case '*':
            tokenizer->outputToken.value = "*";
            return;
        case '<':
            tokenizer->outputToken.value = "<";
            state_SecondEq(tokenizer);
            return;
        case '>':
            tokenizer->outputToken.value = ">";
            state_SecondEq(tokenizer);
            return;
        case ')':
            tokenizer->outputToken.value = ")";
            return;
        case '(':
            tokenizer->outputToken.value = "(";
            return;
        case '{':
            tokenizer->outputToken.value = "{";
            return;
        case '}':
            tokenizer->outputToken.value = "}";
            return;
        case ',':
            tokenizer->outputToken.value = ",";
            return;
        case ';':
            tokenizer->outputToken.value = ";";
            return;
        case '=':
            tokenizer->outputToken.value = "=";
            state_SecondEq(tokenizer);
            return;
        case ':':
            if (state_SecondEq(tokenizer) == 1) {
                tokenizer->errorCode = 1;
                return;
            }
            return;
        case '!':
            if (state_SecondEq(tokenizer) == 1) {
                tokenizer->errorCode = 1;
                return;
            }
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
            tokenizer->outputToken.type = t_NONE;
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
			tokenizer->outputToken.type = t_KW;
            return;
		}
    }

	tokenizer->outputToken.type = t_ID;
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
    tokenizer->outputToken.type = t_INT;
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
    tokenizer->outputToken.type = t_DOUBLE;
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
        tokenizer->outputToken.type = t_DOUBLE;
    } else {
        tokenizer->errorCode = 1;
    }
    tokenizer->processed = false;
}

//TODE decode escape sequences
//this is not final
void state_String(tTokenizer* tokenizer){
    cleanBuilder(&tokenizer->sb);
    do {
	    if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
	        tokenizer->errorCode = 99;
            return;
		}
        getNextChar(tokenizer);

        //rune_literals with start backslash 
        if((int)tokenizer->actualChar == 92){
	        if (appendChar(&tokenizer->sb, '\\') == 1){ // add backslash
	            tokenizer->errorCode = 99;
                return;
		    }
            getNextChar(tokenizer);

            if (tokenizer->actualChar == 'x'){
	            if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){ //add x
	                tokenizer->errorCode = 99;
                    return;
		        }
                getNextChar(tokenizer);

                for(int i = 0; i < 2; i++){
                    if(!isActHexNum(tokenizer)){
                        tokenizer->errorCode = 1;
                        return;
                    }
	                if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){ //add first and second hex
	                    tokenizer->errorCode = 99;
                        return;
		            }
                    getNextChar(tokenizer);
                }
            } else if (!isRuneLitaral(tokenizer)){
                tokenizer->errorCode = 1;
                return;
            }
        }


        //return valid string token
        if((int)tokenizer->actualChar == 34){
	        if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){ //add "
	            tokenizer->errorCode = 99;
                return;
		    }
	        if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		        tokenizer->errorCode = 99;
                return;
	        }
            tokenizer->outputToken.type = t_STRING;
            tokenizer->processed = true;
            return;
        }
    } while ((int)tokenizer->actualChar > 31 && (int)tokenizer->actualChar != 34);
    tokenizer->errorCode = 1;
    return;
}

void state_EOL(tTokenizer* tokenizer){
    if (tokenizer->eolFlag == EOL_FORBIDEN){
        tokenizer->errorCode = 1;
    } else {
        tokenizer->outputToken.value = "";
        tokenizer->outputToken.type = t_EOL;
    }
    getNextChar(tokenizer);
    tokenizer->processed = false;
}

int state_OneLineComment(tTokenizer* tokenizer){
    getNextChar(tokenizer);
    if(tokenizer->actualChar == '/'){
        do {
            getNextChar(tokenizer);
        } while(tokenizer->actualChar != '\n');
        getNextChar(tokenizer);
    } else if (tokenizer->actualChar == '*'){ 
        if (state_BlockComment(tokenizer) != 0) {
            return 1;
        }
    } else {
        tokenizer->outputToken.value = "/";
        tokenizer->outputToken.type = t_NONE;
        tokenizer->processed = false;
        return 1;
    }
    return 0;
}

int state_BlockComment(tTokenizer* tokenizer) {
    do {
        getNextChar(tokenizer);
        if (tokenizer->actualChar == 1){
            tokenizer->errorCode = 1;
            return 1;
        }
    } while(tokenizer->actualChar != '*');
    getNextChar(tokenizer);
    if (tokenizer->actualChar != '/'){
        tokenizer->errorCode = 1;
        return 1;
    }
    getNextChar(tokenizer);
    return 0;
}

int state_SecondEq(tTokenizer* tokenizer){
    cleanBuilder(&tokenizer->sb);
    if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
		tokenizer->errorCode = 99;
        return 1;
	}
    getNextChar(tokenizer);
    tokenizer->processed = false;
    if (tokenizer->actualChar == '=') {
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return 1;
		}
	    if (getStringFromBuilder(&tokenizer->sb, &tokenizer->outputToken.value) == 1){
		    tokenizer->errorCode = 99;
            return 1;
	    }
        tokenizer->processed = true;
        return 0;
    }
    return 1;
}