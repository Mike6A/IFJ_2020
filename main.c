#include "scanner.h"
#include "symtable/symtable.h"

//eg.
extern char* KEYWORDS[];

char* getEnumString(TokenType type){
    switch (type)
    {
        case tokenType_ID: return "ID";
        case tokenType_KW: return "KW";
        case tokenType_INT: return "INT";
        case tokenType_DOUBLE: return "DOUBLE";
        case tokenType_STRING: return "STRING";
        case tokenType_NONE: return "NONE";
        case tokenType_EOF: return "EOF";
        default: return "";
    }
}

void test_tokenizer(){
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);
    do {
        getToken(&tokenizer); 
        printf("%20s | %6s | err: %d\n", tokenizer.outputToken.value, getEnumString(tokenizer.outputToken.type), tokenizer.errorCode);

    } while (tokenizer.outputToken.type != tokenType_EOF);
    destructBuilder(&tokenizer.sb);
}

void test_hashtable(){
    tHashTable table;
    initHashTable(&table, 10);
    addDataToHT(&table, "myVar", "25", true);
    addDataToHT(&table, "mysecondVar", "10", true);
    addDataToHT(&table, "myVar1", "25", true);

    tHashItem* item = getHashItem(&table, "mysecondVar");
    printf("%s, %s\n", item->id, item->value);

    removeHashItem(&table, "mysecondVar");
    removeHashItem(&table, "myVar");
    removeHashItem(&table, "myVar1");
    destructHashTable(&table);
}

void test_tree(){
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);

    getToken(&tokenizer);
    SyntaxNode* exp = ParseExpression(&tokenizer, 0);
    printSyntaxTree(exp, "", true);
    long res = eval(&tokenizer, exp);
    printf("RES>>> %ld", res);
    deleteSyntaxTree(exp);
    destructBuilder(&tokenizer.sb);

}

void test_GetTOKEN() {
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);
    do {
        getToken(&tokenizer);
        printf("%s\n", tokenizer.outputToken.value);
    } while (tokenizer.outputToken.type != tokenType_EOF);

    destructBuilder(&tokenizer.sb);
}

int main() {
    test_tree();
	return 1;
}
