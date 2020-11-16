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
    tokenizer->errorCode = 0;
    tokenizer->isEOF = false;
    tokenizer->processed = true;
    tokenizer->actualChar = ' ';
    
    tToken token = {.value = "", .type = tokenType_NONE};
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
    tokenizer->outputToken.type = tokenType_EOF;
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
    freeToken(tokenizer);
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
        state_EOF(tokenizer);
        return;
    }

    if(tokenizer->actualChar == '/'){ //recognize if is comment or divide operator
        int res_OLC = state_OneLineComment(tokenizer);
        if (res_OLC == 0) {
            state_EOL(tokenizer);
            getNextChar(tokenizer);
            tokenizer->processed = false;
            return;
        } else if (res_OLC != 3)
            return;
    }


    while (isSeparator(tokenizer) || tokenizer->actualChar == '\n' || tokenizer->actualChar == '\r' || tokenizer->actualChar == '/'){ //skip all white spaces
        if (tokenizer->actualChar == '\n') {
            if (state_EOL(tokenizer) == 0){
                getNextChar(tokenizer);
                tokenizer->processed = false;
                return;
            }
        }
        else if(tokenizer->actualChar == '/'){ //recognize if is comment or divide operator
            int res_OLC = state_OneLineComment(tokenizer);
            if (res_OLC == 0) {
                state_EOL(tokenizer);
                getNextChar(tokenizer);
                tokenizer->processed = false;
                return;
            } else if (res_OLC != 3)
                return;
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
    tokenizer->processed = true;
    switch (tokenizer->actualChar) {
        case '+':
            tokenizer->outputToken.value = "+";
            tokenizer->outputToken.type = tokenType_PLUS;
            return;
        case '-':
            tokenizer->outputToken.value = "-";
            tokenizer->outputToken.type = tokenType_MINUS;
            return;
        case '*':
            tokenizer->outputToken.value = "*";
            tokenizer->outputToken.type = tokenType_MUL;
            return;
        case '<':
            tokenizer->outputToken.value = "<";
            if (state_SecondEq(tokenizer) == 1)
                tokenizer->outputToken.type = tokenType_LESS;
            else 
                tokenizer->outputToken.type = tokenType_LE;
            return;
        case '>':
            tokenizer->outputToken.value = ">";
            if (state_SecondEq(tokenizer) == 1)
                tokenizer->outputToken.type = tokenType_GREATER;
            else 
                tokenizer->outputToken.type = tokenType_GE;
            return;
        case ')':
            tokenizer->outputToken.value = ")";
            tokenizer->outputToken.type = tokenType_RBN;
            return;
        case '(':
            tokenizer->outputToken.value = "(";
            tokenizer->outputToken.type = tokenType_LBN;
            return;
        case '{':
            tokenizer->outputToken.value = "{";
            tokenizer->outputToken.type = tokenType_LBC;
            return;
        case '}':
            tokenizer->outputToken.value = "}";
            tokenizer->outputToken.type = tokenType_RBC;
            return;
        case ',':
            tokenizer->outputToken.value = ",";
            tokenizer->outputToken.type = tokenType_COMMA;
            return;
        case ';':
            tokenizer->outputToken.value = ";";
            tokenizer->outputToken.type = tokenType_SCOMMA;
            return;
        case '=':
            tokenizer->outputToken.value = "=";
            if (state_SecondEq(tokenizer) == 1)
                tokenizer->outputToken.type = tokenType_ASSIGN;
            else  
                tokenizer->outputToken.type = tokenType_EQ;
            return;
        case ':':
            if (state_SecondEq(tokenizer) == 1) {
                tokenizer->outputToken.value = "";
                tokenizer->errorCode = 1;
                return;
            }
            tokenizer->outputToken.type = tokenType_DECL;
            return;
        case '!':
            if (state_SecondEq(tokenizer) == 1) {
                tokenizer->outputToken.value = "";
                tokenizer->errorCode = 1;
                return;
            }
            tokenizer->outputToken.type = tokenType_NEQ;
            return;
        default:
            tokenizer->errorCode = 1;
            tokenizer->outputToken.type = tokenType_NONE;
            return;
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
			tokenizer->outputToken.type = tokenType_KW;
            return;
		}
    }

	tokenizer->outputToken.type = tokenType_ID;
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
    tokenizer->outputToken.type = tokenType_INT;
    tokenizer->processed = false;
}

/**
 * @brief This function hanle basic double state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 */
void state_BasicDouble(tTokenizer* tokenizer){
    bool mandatoryNum = false;
    do {
		if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
			tokenizer->errorCode = 99;
            return;
		}
  		getNextChar(tokenizer);
        if (isActNumber(tokenizer))
            mandatoryNum = true;
    } while (isActNumber(tokenizer));

    if (!mandatoryNum){
        tokenizer->outputToken.value = "";
        tokenizer->errorCode = 1;
        return;
    }
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
    tokenizer->outputToken.type = tokenType_DOUBLE;
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
        tokenizer->outputToken.type = tokenType_DOUBLE;
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
    bool bckslhQMARK = false;
    do {
        if (!bckslhQMARK){
            if (appendChar(&tokenizer->sb, tokenizer->actualChar) == 1){
                tokenizer->errorCode = 99;
                return;
            }
            getNextChar(tokenizer);
        }
        bckslhQMARK = false;

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
            } else if (tokenizer->actualChar == '\"'){
	            if (appendChar(&tokenizer->sb, '\"') == 1){ //add first and second hex
	                tokenizer->errorCode = 99;
                    return;
		        }
                getNextChar(tokenizer);
                bckslhQMARK = true;
                continue;
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
            tokenizer->outputToken.type = tokenType_STRING;
            tokenizer->processed = true;
            return;
        }
    } while ((int)tokenizer->actualChar > 31);
    tokenizer->errorCode = 1; //if is EOF set error 
    tokenizer->outputToken.value = "";
    return;
}

/**
 * @brief This function make EOL token.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 
 */
int state_EOL(tTokenizer* tokenizer){
    tokenizer->outputToken.type = tokenType_EOL;
    tokenizer->outputToken.value = "";
    return 0;
}

/**
 * @brief This function hanle one line comment state and divide operator
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 if is one line comment correct
 * \n 1 if is it divide operator
 * \n 2 if is block comment uncorrect 
 * \n 3 if is block comment correct 
 */
int state_OneLineComment(tTokenizer* tokenizer){
    getNextChar(tokenizer);
    if(tokenizer->actualChar == '/'){ //is one line comment
        do {
            getNextChar(tokenizer);
        } while(tokenizer->actualChar != '\n' && tokenizer->actualChar != '\r' && !tokenizer->isEOF);
    } else if (tokenizer->actualChar == '*'){  //is block comment
        if (state_BlockComment(tokenizer) != 0) {
            return 2;
        }
        return 3;
    } else { //is divede operator
        tokenizer->outputToken.value = "/";
        tokenizer->outputToken.type = tokenType_DIV;
        tokenizer->processed = false;
        return 1;
    }
    return 0;
}

/**
 * @brief This function hanle block comment state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 if is correct block comment
 */
int state_BlockComment(tTokenizer* tokenizer) {
    bool star = false;
    do {
        if (!star)
            getNextChar(tokenizer);
        star = false;
        if (tokenizer->actualChar == 1){ //if is EOF sets error
            tokenizer->outputToken.value = "";
            tokenizer->errorCode = 1;
            return 1;
        }
        if (tokenizer->actualChar == '*'){ //if is EOF sets error
            getNextChar(tokenizer);
            star = true;
            if (tokenizer->actualChar == '/'){ //if is EOF sets error
                break;
            } else if (tokenizer->actualChar == 1){ //if is EOF sets error
                tokenizer->outputToken.value = "";
                tokenizer->errorCode = 1;
                return 1;
            }
        }
    } while(tokenizer->actualChar > 1);
    return 0;
}

/**
 * @brief This function handle second char is equal state.
 * 
 * @param tokenizer valid pointer to Tokenizer struct.
 * @return 0 if second char is =
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


void freeToken(tTokenizer* tokenizer) {
    if (tokenizer->errorCode > 0) 
        return;
    if (tokenizer->outputToken.type <= 9) {
        if (tokenizer->outputToken.value != NULL && strlen(tokenizer->outputToken.value) > 0)
            free(tokenizer->outputToken.value);
    }
}