#include "scanner.h"

//eg.
extern char* KEYWORDS[];

char* getEnumString(TokenType type){
    switch (type)
    {
        case t_ID: return "ID";
        case t_KW: return "KW";
        case t_INT: return "INT";
        case t_DOUBLE: return "DOUBLE";
        case t_STRING: return "STRING";
        case t_NONE: return "NONE";
        case t_EOF: return "EOF";
        default: return "";
    }
}

int main() {
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);
    do {
        getToken(&tokenizer); 
        printf("%20s | %6s | err: %d\n", tokenizer.outputToken.value, getEnumString(tokenizer.outputToken.type), tokenizer.errorCode); 
        if (tokenizer.outputToken.type != t_EOF && tokenizer.errorCode == 0) {
            if (tokenizer.outputToken.type != t_NONE){
                free(tokenizer.outputToken.value);
            } else if (strlen(tokenizer.outputToken.value) > 1) {
                free(tokenizer.outputToken.value);
            }
        }
    } while (tokenizer.outputToken.type != t_EOF);
    destructBuilder(&tokenizer.sb);
	return 0;
}