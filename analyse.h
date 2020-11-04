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
    Node_NumberExpression,
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

    //Tokens
    Node_NumberIntToken,
    Node_NumberFloatToken,
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

    Node_FunctionParameter,
    Node_FunctionCallParameters,
};
static char *enumTypeOfNode[] = {
        "Node_Global",

        "Node_NumberExpression",
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
        //TOKENST
        "Node_NumberIntToken",
        "Node_NumberFloatToken",
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

SyntaxNode* ParseExpression(tTokenizer* tokenizer, int priority, tScope* scope);
SyntaxNodes* ParseGlobalBlockExpressions (tTokenizer* tokenizer, int parentPriority, tScope* scope);
SyntaxNodes* ParseBlockExpressions(tTokenizer* tokenizer, int priority, tScope* scope);
long eval(tTokenizer* tokenizer, SyntaxNode * root, tScope* scope);
void deleteSyntaxTree(SyntaxNode* node);
#endif //IFJ_2020_SYNTAXTREE_H
