#include "scanner.h"
#include "symtable/symtable.h"
#include "analyse.h"

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
        case tokenType_PLUS: return "PLUS";
        case tokenType_MINUS: return "MINUS";
        case tokenType_MUL: return "MUL";
        case tokenType_DIV: return "DIV";
        case tokenType_LESS: return "LESS";
        case tokenType_GREATER: return "GREATER";
        case tokenType_LE: return "LE";
        case tokenType_GE: return "GE";
        case tokenType_EQ: return "EQ";
        case tokenType_NEQ: return "NEQ";
        case tokenType_LBN: return "LBN";
        case tokenType_RBN: return "RBN";
        case tokenType_LBC: return "LBC";
        case tokenType_RBC: return "RBC";
        case tokenType_COMMA: return "COMMA";
        case tokenType_SCOMMA: return "SCAMMA";
        case tokenType_DECL: return "DECL";
        case tokenType_ASSIGN: return "ASSIGN";
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
    tScope scope;
    initScope(&scope);
    createScope(&scope);
    getToken(&tokenizer);
    SyntaxNode* prog;
    prog = createNode(NULL, NULL, NULL, NULL, "GlobalScope", Node_Global);
    prog->statements = ParseGlobalBlockExpressions(&tokenizer, 0, &scope);
    printSyntaxTree(prog, "", true);
    //long res = eval(&tokenizer, prog, &scope);
    deleteSyntaxTree(prog);
//    while (!tokenizer.isEOF){
//        getToken(&tokenizer);
//        SyntaxNode* exp = ParseExpression(&tokenizer, 0, &scope);
//        /*if(exp == NULL)
//            printf("EXPRESSION NULL");
//            */
//        printSyntaxTree(exp, "", true);
//        long res = eval(&tokenizer, exp, &scope);
//        if(res!=0)
//            printf("RES>>> %ld\n", res);
//        deleteSyntaxTree(exp);
//    }
    removeLastLocalScope(&scope);
    destructBuilder(&tokenizer.sb);

}

void test_GetTOKEN() {
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);
    do {
        getToken(&tokenizer);
        printf("%s\t%s\n", tokenizer.outputToken.value, getEnumString(tokenizer.outputToken.type));

    } while (tokenizer.outputToken.type != tokenType_EOF);

    destructBuilder(&tokenizer.sb);
}

int main() {
    test_tree();
	return 1;
}
