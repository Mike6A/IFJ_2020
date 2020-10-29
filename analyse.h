//
// Created by david on 27/10/2020.
//
#include "scanner.h"
#ifndef IFJ_2020_SYNTAXTREE_H
#define IFJ_2020_SYNTAXTREE_H
enum typeOfNode{
    //Expressions
    Node_NumberExpression,
    Node_ParenthezedExpression,
    Node_BinaryExpression,
    Node_UnaryExpression,
    Node_BooleanExpression,

    //Tokens
    Node_NumberIntToken,
    Node_NumberFloatToken,
    Node_OpenParenthesisToken,
    Node_CloseParenthesisToken,
    Node_OperatorToken
};
typedef struct t_syntaxTree {
    struct t_syntaxTree* left;
    struct t_syntaxTree* operator;
    struct t_syntaxTree* right;
    tToken* token;
    enum typeOfNode type;
    char* name;
}SyntaxNode;

void initSyntaxNode(SyntaxNode *root);
void printSyntaxTree(SyntaxNode* node, char* indent, bool last);
SyntaxNode* ParseExpression(tTokenizer* tokenizer, int priority);
long eval(tTokenizer* tokenizer, SyntaxNode * root);
void deleteSyntaxTree(SyntaxNode* node);
#endif //IFJ_2020_SYNTAXTREE_H
