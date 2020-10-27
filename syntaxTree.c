/**
 * @file scenner.c
 * @author Dávid Oravec (xorave06@stud.fit.vutbr.cz)
 * @brief Implementation of syntaxTree for IFJ20.
 */

#include "syntaxTree.h"

void initSyntaxNode(SyntaxNode* root){
    root->left = NULL;
    root->operator = NULL;
    root->right = NULL;
    root->token = NULL;
    root->name = "";
}
SyntaxNode* createNodeFromToken(tToken* token, char* name){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->token = token;
    current->name = name;
    return current;
}
tToken* CopyToken(tToken* token){
    tToken* newToken = malloc(sizeof(tToken));
    newToken->value = (char*)malloc(sizeof(char) * strlen(token->value) + 1);
    strcpy(newToken->value, token->value);
    newToken->type = token->type;
    return newToken;
}

tToken* Match(tTokenizer* tokenizer, int type){
    tToken* current = NULL;
    if(tokenizer->outputToken.type == type){
        current = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);

    }
        //@TODO Add error Return ... expected something else.
    return current;
}

SyntaxNode* numberExpressionSyntax(tToken* numberToken){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->operator = createNodeFromToken(numberToken, "NumberToken");
    current->token = NULL;
    current->name = "NumberExpression";
    return current;
}

SyntaxNode* binaryExpressionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->right = right;
    current->operator = NULL;
    current->left = left;
    current->token = operator;
    current->name = "BinaryExpression";
    return current;
}
SyntaxNode* parenthezedExpressionSyntax(tToken* left, SyntaxNode* expression, tToken* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(left, "OpenParenthesisToken");
    current->operator = expression;
    current->token = NULL;
    current->right = createNodeFromToken(right, "CloseParenthesisToken");
    current->name = "ParenthezedExpression";
    return current;
}

SyntaxNode* PrimaryExpressionSyntax(tTokenizer* tokenizer){
    if(tokenizer->outputToken.type == tokenType_LBN){
        tToken *left = Match(tokenizer, tokenType_LBN);
        SyntaxNode *expression = ParseExpression(tokenizer);
        tToken *right = Match(tokenizer, tokenType_RBN);
        return parenthezedExpressionSyntax(left, expression, right);
    }
    tToken* numberToken = Match(tokenizer, tokenType_INT);
    SyntaxNode *node = numberExpressionSyntax(numberToken);
    return node;
}

SyntaxNode* ParseExpression(tTokenizer* tokenizer){
    //@todo ADD *,/, true, false, unary operators, +,-,!.
    //@todo ADD Error report...
    SyntaxNode* left = PrimaryExpressionSyntax(tokenizer);
    while (tokenizer->outputToken.type == tokenType_PLUS || tokenizer->outputToken.type == tokenType_MINUS){

        tToken* operator = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);

        SyntaxNode* right = PrimaryExpressionSyntax(tokenizer);
        left = binaryExpressionSyntax(left, operator, right);
    }
    return left;
}
void printSyntaxTree(SyntaxNode* node, char* indent, bool last){
    if(node == NULL)
        return;

    char* marker = last ? "└──" : "├──";
    printf("%s%s", indent,marker);
    printf("%s", node->name);
    if(node->token != NULL && node->token->value != NULL){
        printf(" %s", node->token->value);
    }
    printf("\n");
    char newIndent[1024];
    strcpy(newIndent, indent);
    strcat(newIndent, last ? "   " : "│  ");
    printSyntaxTree(node->left, newIndent, node->operator == NULL && node->right == NULL);
    printSyntaxTree(node->operator, newIndent, node->right == NULL);
    printSyntaxTree(node->right, newIndent, true);
}

void deleteSyntaxTree(SyntaxNode* node){
    if(node == NULL)
        return;
    //printSyntaxTree(node, "", true);
    if(node->token != NULL){
        if(node->token->value != NULL){
            free(node->token->value);
        }
        free(node->token);
    }
    deleteSyntaxTree(node->left);
    deleteSyntaxTree(node->operator);
    deleteSyntaxTree(node->right);
    free(node);

}