/**
 * @file Analyse.c
 * @author Dávid Oravec (xorave06@stud.fit.vutbr.cz)
 * @brief Implementation of syntaxTree for IFJ20.
 */

#include "analyse.h"

//@TODO Proper NAMING AND SEPARATE FILES!!!
void initScope(tScope* scope){
    scope->table = NULL;
    scope->next = NULL;
    scope->top = NULL;
}

int createScope(tScope *scope){
    tHashTable* table = (tHashTable*) malloc(sizeof(tHashTable));
    initHashTable(table, MAX_HASHTABLE_SIZE);
    if(table == NULL) {
        free(table);
        return 1;
    }
    if(scope->table == NULL){
        scope->top = scope;
        scope->table = table;
    }else{
        tScope* newScope = (tScope*)malloc(sizeof(tScope));
        if(newScope == NULL) {
            free(table);
            return 1;
        }
        newScope->next = scope->top;
        newScope->table = table;
        scope->top = newScope;
    }
    return 0;
}

int removeScope(tScope* scope){
    tScope *tmp = scope->top;
    scope->top = tmp->next;
    free(tmp->table);
    free(tmp);
    return 0;
}


char* getEnumString2(TokenType type){
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
        if(tokenizer->actualChar != '\n' && tokenizer->actualChar != '\r'){
            getToken(tokenizer);
        }

    }else{
        fprintf(stderr,"Expected: %s \t--- Given: %s\n", getEnumString2(type), getEnumString2(tokenizer->outputToken.type));
        exit(2);
    }
    return current;
}

int GetUnOperatorPriority(int tokenType){
    switch (tokenType) {
        case tokenType_PLUS:
        case tokenType_MINUS:
            return 4;
        default:
            return 0;
    }
}

int GetBinOperatorPriority(int tokenType){
    switch (tokenType) {
        case tokenType_GREATER:
        case tokenType_LESS:
        case tokenType_LE:
        case tokenType_EQ:
        case tokenType_GE:
        case tokenType_NEQ:
            return 3;
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

SyntaxNode* identifierExpressionSyntax(tToken* identifier){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->operator = createNodeFromToken(identifier, "identifier", Node_IdentifierToken);
    current->type = Node_IdentifierExpression;
    current->name = "IdentifierExpression";
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
SyntaxNode* unaryExpressionSyntax(tToken* operator, SyntaxNode* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->right = right;
    current->operator = NULL;
    current->left = NULL;
    current->token = operator;
    current->type = Node_UnaryExpression;
    current->name = "UnaryExpression";
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


SyntaxNode* assignExpressionSyntax(tToken* identifier, tToken* equalsToken, SyntaxNode* expression){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(identifier, "identifier", Node_IdentifierToken);
    current->operator = NULL;
    current->token = equalsToken;
    current->right = expression;
    current->type = Node_AssignmentExpression;
    current->name = "AssignExpression";
    return current;
}

SyntaxNode* PrimaryExpressionSyntax(tTokenizer* tokenizer, tScope* scope){
    // S => (exp)
    if(tokenizer->outputToken.type == tokenType_EOF){
        return NULL;
    }
    if(tokenizer->outputToken.type == tokenType_LBC){
        createScope(scope);
        return NULL;
    }
    if(tokenizer->outputToken.type == tokenType_RBC){
        removeScope(scope);
        return NULL;
    }
    if(tokenizer->outputToken.type == tokenType_LBN){
        tToken *left = Match(tokenizer, tokenType_LBN);
        SyntaxNode *expression = ParseExpression(tokenizer, 0, scope);
        tToken *right = Match(tokenizer, tokenType_RBN);
        return parenthezedExpressionSyntax(left, expression, right);
    }

    if(tokenizer->outputToken.type == tokenType_ID){
        tToken *identifier = Match(tokenizer, tokenType_ID);
        if (tokenizer->outputToken.type == tokenType_DECL || tokenizer->outputToken.type == tokenType_ASSIGN){
            tToken * assing = CopyToken(&tokenizer->outputToken);
            getToken(tokenizer);
            SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
            return assignExpressionSyntax(identifier, assing, expr);
        }
        return identifierExpressionSyntax(identifier);
    }
    if(tokenizer->outputToken.type == tokenType_KW){
        tokenizer->eolFlag = EOL_FORBIDEN;
        tToken *kw = Match(tokenizer, tokenType_KW);
        if(strcmp(kw->value, "if") == 0){
            return NULL;
        }
    }
    //S => INT
    tToken* numberToken = Match(tokenizer, tokenType_INT);
    SyntaxNode *node = numberExpressionSyntax(numberToken);
    return node;
}

SyntaxNode* ParseExpression(tTokenizer* tokenizer, int parentPriority, tScope* scope){
    //@todo ADD *,/, true, false, unary operators, +,-,!.
    //@todo ADD Error report...
    SyntaxNode* left;
    int unaryPriority = GetUnOperatorPriority(tokenizer->outputToken.type);
    //S => -exp | +exp
    if(unaryPriority != 0 && unaryPriority >= parentPriority){
        tToken* operator = Match(tokenizer, tokenizer->outputToken.type);
        SyntaxNode* operand = ParseExpression(tokenizer, unaryPriority, scope);
        left = unaryExpressionSyntax(operator, operand);
    }else{
        left = PrimaryExpressionSyntax(tokenizer, scope);
    }

    while (tokenizer->actualChar != '\n' && tokenizer->actualChar != '\r'){
        int priority = GetBinOperatorPriority(tokenizer->outputToken.type);
        if(priority == 0 || priority <= parentPriority)
            break;
        //Try to Build Binary expression if exists binary operator
        tToken* operator = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);


        SyntaxNode* right = ParseExpression(tokenizer, priority, scope);
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

long eval(tTokenizer* tokenizer, SyntaxNode * root, tScope* scope){
    if(root == NULL){
        return 0;
    }
    if(root->type == Node_NumberExpression){
        char* end;
        long result = strtol (root->operator->token->value,&end,10);
        return result;
    }
    if(root->type == Node_IdentifierExpression){
        tScope *currentScope = scope->top;
        tHashItem *item;
        do {
            item = getHashItem(currentScope->table, root->operator->token->value);
            if(item == NULL){
                currentScope = currentScope->next;
            }
        }while(item == NULL && currentScope != NULL);

        if(item == NULL){
            fprintf(stderr, "Identifier %s was not declared!", root->operator->token->value);
            exit(3);
        }
        char* end;
        long result = strtol (item->value,&end,10);
        return result;
    }
    if(root->type == Node_AssignmentExpression){
        tHashItem *item = getHashItem(scope->top->table, root->left->token->value);
        if(item == NULL){
            if(root->token->type == tokenType_DECL){
                char value[1024];
                long res = eval(tokenizer, root->right, scope);
                sprintf(value, "%ld", res);
                if(addDataToHT(scope->top->table, root->left->token->value, value, true) == 1){
                    fprintf(stderr, "ERRTABLE\n");
                }
                return res;
            }else if (root->token->type == tokenType_ASSIGN){
                tScope *currentScope = scope->top->next;
                while(item == NULL && currentScope != NULL){
                    item = getHashItem(currentScope->table, root->operator->token->value);
                    if(item == NULL){
                        currentScope = currentScope->next;
                    }
                }
                if(currentScope == NULL){
                    fprintf(stderr, "Identifier %s was not declared! CannotAssign\n", root->left->token->value);
                    exit(3);
                }
                char value[1024];
                long res = eval(tokenizer, root->right, currentScope);
                sprintf(value, "%ld", res);
                strcpy(item->value, value);
                return res;
            }
            fprintf(stderr, "Identifier %s was not declared! CannotAssign\n", root->left->token->value);
            exit(3);
        }
        if(root->token->type == tokenType_ASSIGN){
            char value[1024];
            long res = eval(tokenizer, root->right, scope);
            sprintf(value, "%ld", res);
            strcpy(item->value, value);
            return res;
        }
        fprintf(stderr, "Identifier %s was declared! CannotRedeclare\n", root->left->token->value);
        exit(3);
    }
    if(root->type == Node_UnaryExpression){
        long operand = eval(tokenizer, root->right, scope);
        switch (root->token->type) {
            case tokenType_PLUS:
                return operand;
            case tokenType_MINUS:
                return -operand;
            default:
                return -1;
        }
    }
    if(root->type == Node_BinaryExpression){
        long left = eval(tokenizer, root->left, scope);
        long right = eval(tokenizer, root->right, scope);
        switch (root->token->type) {
            case tokenType_PLUS:
                return left + right;
            case tokenType_MINUS:
                return left - right;
            case tokenType_MUL:
                return left * right;
            case tokenType_DIV:
                return left / right;
            case tokenType_GREATER:
                return left > right;
            case tokenType_LESS:
                return left < right;
            case tokenType_GE:
                return left >= right;
            case tokenType_LE:
                return left <= right;
            case tokenType_EQ:
                return left == right;
            case tokenType_NEQ:
                return left != right;
            default:
                return -1;
        }
    }
    if(root->type == Node_ParenthezedExpression){
        return eval(tokenizer, root->operator, scope);
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
