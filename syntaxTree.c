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
SyntaxNode* createNodeFromToken(tToken* token, char* name, int type){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->token = token;
    current->name = name;
    current->type = type;
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

int GetOperatorPriority(int tokenType){
    switch (tokenType) {
        case tokenType_MUL:
        case tokenType_DIV:
            return 2;
        case tokenType_PLUS:
        case tokenType_MINUS:
            return 1;
        default:
            return 0;
    }
}

SyntaxNode* numberExpressionSyntax(tToken* numberToken){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->operator = createNodeFromToken(numberToken, "NumberToken", Node_NumberIntToken);
    current->token = NULL;
    current->type = Node_NumberExpression;
    current->name = "NumberExpression";
    return current;
}

SyntaxNode* binaryExpressionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->right = right;
    current->operator = NULL;
    current->left = left;
    current->token = operator;
    current->type = Node_BinaryExpression;
    current->name = "BinaryExpression";
    return current;
}
SyntaxNode* parenthezedExpressionSyntax(tToken* left, SyntaxNode* expression, tToken* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(left, "OpenParenthesisToken", Node_OpenParenthesisToken);
    current->operator = expression;
    current->token = NULL;
    current->right = createNodeFromToken(right, "CloseParenthesisToken", Node_CloseParenthesisToken);
    current->type = Node_ParenthezedExpression;
    current->name = "ParenthezedExpression";
    return current;
}

SyntaxNode* PrimaryExpressionSyntax(tTokenizer* tokenizer){
    if(tokenizer->outputToken.type == tokenType_LBN){
        tToken *left = Match(tokenizer, tokenType_LBN);
        SyntaxNode *expression = ParseExpression(tokenizer, 0);
        tToken *right = Match(tokenizer, tokenType_RBN);
        return parenthezedExpressionSyntax(left, expression, right);
    }
    tToken* numberToken = Match(tokenizer, tokenType_INT);
    SyntaxNode *node = numberExpressionSyntax(numberToken);
    return node;
}

SyntaxNode* ParseExpression(tTokenizer* tokenizer, int parentPriority){
    //@todo ADD *,/, true, false, unary operators, +,-,!.
    //@todo ADD Error report...
    SyntaxNode* left = PrimaryExpressionSyntax(tokenizer);
    while (true){
        int priority = GetOperatorPriority(tokenizer->outputToken.type);
        if(priority == 0 || priority <= parentPriority){
            break;
        }
        tToken* operator = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);

        SyntaxNode* right = ParseExpression(tokenizer, priority);
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

long eval(tTokenizer* tokenizer, SyntaxNode * root){
    if(root->type == Node_NumberExpression ){
        char* end;
        long result = strtol (root->operator->token->value,&end,10);
        return result;
    }
    if(root->type == Node_BinaryExpression){
        long left = eval(tokenizer, root->left);
        long right = eval(tokenizer, root->right);
        switch (root->token->type) {
            case tokenType_PLUS:
                return left + right;
            case tokenType_MINUS:
                return left - right;
            case tokenType_MUL:
                return left * right;
            case tokenType_DIV:
                return left / right;
            default:
                return -1;
        }
    }
    if(root->type == Node_ParenthezedExpression){
        return eval(tokenizer, root->operator);
    }
    return -1;
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
