//
// Created by david on 27/10/2020.
//
#include "scanner.h"
#include "symtable/symtable.h"
#ifndef IFJ_2020_SYNTAXTREE_H
#define IFJ_2020_SYNTAXTREE_H
enum typeOfNode{
    Node_Global,
    //Expressions
    Node_NumberIntExpression,
    Node_NumberDoubleExpression,
    Node_StringExpression,
    Node_IdentifierExpression,
    Node_ParenthezedExpression,
    Node_BinaryExpression,
    Node_UnaryExpression,
    Node_DeclareExpression,
    Node_AssignmentExpression,
    Node_AssignmentToExpression,
    Node_AssignmentValuesExpression,
    Node_BlockExpression,
    Node_IFExpression,
    Node_ElseExpression,
    Node_ForExpression,
    Node_ForInitExpressions,
    Node_FunctionExpression,
    Node_FunctionInitExpression,
    Node_FunctionCallExpression,
    Node_ReturnExpression,
    Node_BooleanExpression,
    Node_PackageExpression,

    //Tokens
    Node_NumberIntToken,
    Node_NumberDoubleToken,
    Node_StringToken,
    Node_OpenParenthesisToken,
    Node_CloseParenthesisToken,
    Node_IdentifierToken,
    Node_OpenBlockStatementToken,
    Node_CloseBlockStatementToken,
    Node_KWTypeToken,
    Node_ParamIdentifierToken,
    Node_ParamTypeToken,
    Node_AssignmentToken,
    Node_OperatorToken,
    Node_PackageNameToken,

    Node_FunctionParameter,
    Node_FunctionCallParameters,
};
static char *enumTypeOfNode[] = {
        "Node_Global",

        "Node_NumberIntExpression",
        "Node_NumberDoubleExpression",
        "Node_StringExpression",
        "Node_IdentifierExpression",
        "Node_ParenthezedExpression",
        "Node_BinaryExpression",
        "Node_UnaryExpression",
        "Node_DeclareExpression",
        "Node_AssignmentExpression",
        "Node_AssignmentToExpression",
        "Node_AssignmentValuesExpression",
        "Node_BlockExpression",
        "Node_IFExpression",
        "Node_ElseExpression",
        "Node_ForExpression",
        "Node_ForInitExpressions",
        "Node_FunctionExpression",
        "Node_FunctionInitExpression",
        "Node_FunctionCallExpression",
        "Node_ReturnExpression",
        "Node_BooleanExpression",
        "Node_PackageExpression",
        //TOKENST
        "Node_NumberIntToken",
        "Node_NumberDoubleToken",
        "Node_StringToken",
        "Node_OpenParenthesisToken",
        "Node_CloseParenthesisToken",
        "Node_IdentifierToken",
        "Node_OpenBlockStatementToken",
        "Node_CloseBlockStatementToken",
        "Node_KWTypeToken",
        "Node_ParamIdentifierToken",
        "Node_ParamTypeToken",
        "Node_AssignmentToken",
        "Node_OperatorToken",
        "Node_PackageNameToken",


        "Node_FunctionParameter",
        "Node_FunctionCallParameters",
};
//@TODO MAKE SEPARATE FILES FOR STRUCTURES !!!!
//@TODO AND PROPER NAMING !!!!
struct t_syntaxTree;

typedef struct t_syntaxTrees{
    struct t_syntaxTrees *first;
    struct t_syntaxTree *node;
    struct t_syntaxTrees *next;
    struct t_syntaxTrees *last;
}SyntaxNodes;

typedef struct t_syntaxTree {
    struct t_syntaxTree* left;
    SyntaxNodes* statements;
    struct t_syntaxTree* right;
    tToken* token;
    enum typeOfNode type;
    char* name;
}SyntaxNode;

typedef struct scopeItem{
    tHashTable* table;
    struct scopeItem* next;
} tScopeItem;
typedef struct scope{
    tScopeItem* topLocal;
    tScopeItem* global;
}tScope;
void initScope(tScope* scope);
int createScope(tScope *scope);
int removeLastLocalScope(tScope *scope);

SyntaxNode* createNode(SyntaxNode* left, SyntaxNodes* statements, SyntaxNode* right, tToken* token, const char* name, int type);
SyntaxNode* CopyNode(SyntaxNode* node);
void initSyntaxNode(SyntaxNode *root);
void printSyntaxTree(SyntaxNode* node, char* indent, bool last);

SyntaxNode* getPackage(tTokenizer* tokenizer);
SyntaxNode *getFunctionNode(tTokenizer * tokenizer);
SyntaxNode* ParseExpression(tTokenizer* tokenizer, int priority);
SyntaxNode * getSyntaxGlobal(tTokenizer*tokenizer);
SyntaxNodes* ParseGlobalBlockExpressions (tTokenizer* tokenizer, int parentPriority);
SyntaxNodes* ParseBlockExpressions(tTokenizer* tokenizer, int priority);
long eval(tTokenizer* tokenizer, SyntaxNode * root, tScope* scope);

void deleteToken(tToken* token);
void deleteSyntaxTree(SyntaxNode* node);

bool isError();
int getError();
#endif //IFJ_2020_SYNTAXTREE_H
