/**
 * @file scenner.c
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Implementaion of lexical analysis for IFJ20.
 */
#include "scanner.h"

const char RUNE_LITERALS[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '"', '\''};
const char SEPARATORS[] = {' ', '\t'};
const char* KEYWORDS[] = {"if", "else", "float64", "for", "func", "int", "package", "return", "string"};

/**
 * @brief This function init Tokenizer.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 if allocation was successful 
 */
int initTokenizer(tTokenizer* tokenizer) {
    tokenizer->eolFlag = EOL_OPTIONAL;
    tokenizer->errorCode = 0;
    tokenizer->isEOF = false;
    tokenizer->processed = true;
    tokenizer->actualChar = ' ';
    
    tToken token = {.value = "", .type = t_NONE};
    tokenizer->outputToken = token;

    tStringBuilder sb;
    if (initStringBuilder(&sb) == 1)
        return 1;
    tokenizer->sb = sb; 
    return 0;
}

/**
 * @brief This function destruct Tokenizer.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void destructTokenizer(tTokenizer* tokenizer) {
    destructBuilder(&tokenizer->sb);
}

/**
 * @brief This function get the next char from stdin and put it into the variable actualChar in Tokenizer struct. \n
 *        If feof function wasn't successful, sets tokenizer errorCode to 99. \n
 *        If is EOF, sets actualChar to 1 and isEOF to true in Tokenizer struct. 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
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
 * @brief This function test if actualChar is letter or underscore.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return true is a letter or underscore
 */
bool isActLetter(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return ((actualChar >= 'a' && actualChar <= 'z') || (actualChar >= 'A' && actualChar <= 'Z') || actualChar == '_') ? true : false;
}

/**
 * @brief This function test if actualChar is number.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return true if is number
 */
bool isActNumber(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return (actualChar >= '0' && actualChar <= '9') ? true : false;
}

/**
 * @brief This function test if actualChar corresponds to hex number.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return true if is hex number
 */
bool isActHexNum(tTokenizer* tokenizer) {
    char actualChar = tokenizer->actualChar;
    return ((actualChar >= '0' && actualChar <= '9') ||
     (actualChar >= 'a' && actualChar <= 'f') ||
     (actualChar >= 'A' && actualChar <= 'F'));
}

/**
 * @brief  This function test if actualChar is separator (' ', '\t')
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return true if is separator
 */
bool isSeparator(tTokenizer* tokenizer){
    for (int i = 0; i < sizeof(SEPARATORS) / sizeof(SEPARATORS[0]); i++)
        if (tokenizer->actualChar == SEPARATORS[i]) 
            return true;
    return false;
}

/**
 * @brief This function test if actualChar is rune litteral.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return true if is rune litteral
 */
bool isRuneLitaral(tTokenizer* tokenizer){
    for (int i = 0; i < sizeof(RUNE_LITERALS) / sizeof(RUNE_LITERALS[0]); i++)
        if (tokenizer->actualChar == RUNE_LITERALS[i]) 
            return true;
    return false;
}

/**
 * @brief This function represents state EOF and sets output token type to t_EOF.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void state_EOF(tTokenizer* tokenizer){
    tokenizer->outputToken.value = "";
    tokenizer->outputToken.type = t_EOF;
}

/**
 * @brief Get next token from input. \n
 *        Sets outputToken, errorCode, isEof in Tokenizer struct. \n
 *        If error occurs then errorCode is setted and scanner go to the end of stdin.
 * @pre eolFlag in struct must be set.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void getToken(tTokenizer* tokenizer) {
    if (tokenizer->errorCode > 0) {
        while (!tokenizer->isEOF){ //skip all 
            getNextChar(tokenizer);
        }
        state_EOF(tokenizer);
        return;
    }

    if (tokenizer->processed) //if actualChar was recognized
        getNextChar(tokenizer);

    if (tokenizer->isEOF){
        return;
    }

    if (tokenizer->eolFlag == EOL_REQUIRED){ //if end of line is required
        state_EOLRequired(tokenizer);
        return;
    }

    while (isSeparator(tokenizer) || tokenizer->actualChar == '\n' || tokenizer->actualChar == '\r'){ //skip all white spaces
        if (tokenizer->actualChar == '\n') {
            if (state_EOL(tokenizer) == 1){
                return;
            }
        }
        getNextChar(tokenizer);
    }

    if(tokenizer->actualChar == '/'){ //recognize if is comment or divide operator
        if (state_OneLineComment(tokenizer) != 0){
            return;
        }
    }


    while (isSeparator(tokenizer) || tokenizer->actualChar == '\n' || tokenizer->actualChar == '\r'){ //skip all white spaces
        if (tokenizer->actualChar == '\n') {
            if (state_EOL(tokenizer) == 1){
                return;
            }
        }
        getNextChar(tokenizer);
    }

    if (tokenizer->isEOF){ //if is end of stdin
        state_EOF(tokenizer);
        return;
    }
    
    if (isActLetter(tokenizer)){ //recognize id
        state_ID(tokenizer);
        return;
    }

    if (isActNumber(tokenizer)){ //recognize number -> int and double
        state_Num(tokenizer);
        return;
    }

    if(tokenizer->actualChar == '"'){ //recognize string
        state_String(tokenizer);
        return;
    }

    //recognize operators
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
                tokenizer->outputToken.value = "";
                tokenizer->errorCode = 1;
                return;
            }
            return;
        case '!':
            if (state_SecondEq(tokenizer) == 1) {
                tokenizer->outputToken.value = "";
                tokenizer->errorCode = 1;
                return;
            }
            return;
    }
}

/**
 * @brief This function hanle EOL requiered state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void state_EOLRequired(tTokenizer* tokenizer) {
    //check if is eol required
    if (tokenizer->eolFlag == EOL_REQUIRED){
        while (isSeparator(tokenizer)){
            getNextChar(tokenizer);
        }
        if (tokenizer->actualChar != '\n') {
            tokenizer->outputToken.value = "";
            tokenizer->errorCode = 1;
        }
        tokenizer->eolFlag = EOL_OPTIONAL;
    }
}

/**
 * @brief This function hanle ID state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
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

/**
 * @brief This function hanle Num state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
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

/**
 * @brief This function hanle basic double state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void state_BasicDouble(tTokenizer* tokenizer){
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

/**
 * @brief This function hanle exp double state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
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
        tokenizer->outputToken.value = "";
        tokenizer->errorCode = 1;
    }
    tokenizer->processed = false;
}

/**
 * @brief This function hanle string state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * 
 * @todo maybe decode rune literals and hex chars => this is question
 */
void state_String(tTokenizer* tokenizer){
    cleanBuilder(&tokenizer->sb);
    do {
	    if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
	        tokenizer->errorCode = 99;
            return;
		}
        getNextChar(tokenizer);

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

                for(int i = 0; i < 2; i++){ //get two hex nums
                    if(!isActHexNum(tokenizer)){
                        tokenizer->outputToken.value = "";
                        tokenizer->errorCode = 1;
                        return;
                    }
	                if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){ //add first and second hex
	                    tokenizer->errorCode = 99;
                        return;
		            }
                    getNextChar(tokenizer);
                }
            } else if (!isRuneLitaral(tokenizer)){ //rune literals with start backslash 
                tokenizer->outputToken.value = "";
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
    tokenizer->errorCode = 1; //if is EOF set error 
    tokenizer->outputToken.value = "";
    return;
}

/**
 * @brief This function hanle EOL state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 if EOL is allowed
 */
int state_EOL(tTokenizer* tokenizer){
    if (tokenizer->eolFlag == EOL_FORBIDEN){
        tokenizer->errorCode = 1;
        tokenizer->outputToken.value = "";
        return 1;
    }
    return 0;
}

/**
 * @brief This function hanle one line comment state and divide operator
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
int state_OneLineComment(tTokenizer* tokenizer){
    getNextChar(tokenizer);
    if(tokenizer->actualChar == '/'){ //is one line comment
        do {
            getNextChar(tokenizer);
        } while(tokenizer->actualChar != '\n');
        getNextChar(tokenizer);
    } else if (tokenizer->actualChar == '*'){  //is block comment
        if (state_BlockComment(tokenizer) != 0) {
            return 1;
        }
    } else { //is divede operator
        tokenizer->outputToken.value = "/";
        tokenizer->outputToken.type = t_NONE;
        tokenizer->processed = false;
        return 1;
    }
    return 0;
}

/**
 * @brief This function hanle block comment state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
int state_BlockComment(tTokenizer* tokenizer) {
    do {
        getNextChar(tokenizer);
        if (tokenizer->actualChar == 1){ //if is EOF sets error
            tokenizer->outputToken.value = "";
            tokenizer->errorCode = 1;
            return 1;
        }
    } while(tokenizer->actualChar != '*');
    getNextChar(tokenizer);
    if (tokenizer->actualChar != '/'){ //check if correctly ended block comment
        tokenizer->errorCode = 1;
        tokenizer->outputToken.value = "";
        return 1;
    }
    getNextChar(tokenizer);
    return 0;
}

/**
 * @brief This function hanle second char is equal state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
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