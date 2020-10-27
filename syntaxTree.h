//
// Created by david on 27/10/2020.
//
#include "scanner.h"
#ifndef IFJ_2020_SYNTAXTREE_H
#define IFJ_2020_SYNTAXTREE_H

typedef struct t_syntaxTree {
    struct t_syntaxTree* left;
    struct t_syntaxTree* operator;
    struct t_syntaxTree* right;
    tToken* token;
    char* name;
}SyntaxNode;

void initSyntaxNode(SyntaxNode *root);
void printSyntaxTree(SyntaxNode* node, char* indent, bool last);
SyntaxNode* ParseExpression(tTokenizer* tokenizer);
void deleteSyntaxTree(SyntaxNode* node);
#endif //IFJ_2020_SYNTAXTREE_H
