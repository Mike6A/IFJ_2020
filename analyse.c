/**
 * @file Analyse.c
 * @author Dávid Oravec (xorave06@stud.fit.vutbr.cz)
 * @brief Implementation of syntaxTree for IFJ20.
 */

#include "analyse.h"

//@TODO Proper NAMING AND SEPARATE FILES!!!
void initScope(tScope* scope){
    scope->topLocal = NULL;
    scope->global = NULL;
}

int createScope(tScope *scope){
    tHashTable* table = (tHashTable*) malloc(sizeof(tHashTable));
    initHashTable(table, MAX_HASHTABLE_SIZE);
    if(table == NULL) {
        //free(table);
        return 1;
    }
    if(scope->topLocal == NULL){
        tScopeItem* newScope = (tScopeItem *)malloc(sizeof(tScopeItem));
        newScope->next = NULL;
        scope->topLocal = newScope;
        scope->topLocal->table = table;
        scope->global = newScope;
    }else{
        tScopeItem* newScope = (tScopeItem *)malloc(sizeof(tScopeItem));
        if(newScope == NULL) {
            free(table);
            return 1;
        }
        newScope->next = scope->topLocal;
        newScope->table = table;
        scope->topLocal = newScope;
    }
    return 0;
}

int removeLastLocalScope(tScope* scope){
    tScopeItem *tmp = scope->topLocal;
    scope->topLocal = tmp->next;
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
    root->statements = NULL;
    root->right = NULL;
    root->token = NULL;
    root->name = "";
}
SyntaxNodes* createNodeList(SyntaxNode* node){
    SyntaxNodes * list = (SyntaxNodes*)malloc(sizeof(SyntaxNodes));
    if(list != NULL) {
        list->first = list;
        list->node = node;
        list->next = NULL;
        list->last = list;
    }
    return list;
}
int addToNodeListEnd(SyntaxNodes* list, SyntaxNode* next){
    SyntaxNodes * nextListItem = createNodeList(next);
    if(nextListItem == NULL){
        return 1;
    }
    list->first = list;
    list->last->next = nextListItem;
    list->last = nextListItem;
    return 0;
}
int destroyNodeList(SyntaxNodes* list){
    SyntaxNodes * current = list->first;
    while(current != NULL){
        SyntaxNodes * tmp = current;
        free(current);
        tmp->first = NULL;
        tmp->last = NULL;
        free(tmp->node);
        tmp->node = NULL;
        current = tmp->next;
    }
    return 0;
}

SyntaxNode* createNode(SyntaxNode* left, SyntaxNodes* statements, SyntaxNode* right, tToken* token, const char* name, int type){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = left;
    current->statements = statements;
    current->right = right;
    current->token = token;
    current->name = (char*)malloc(sizeof(char) * strlen(name) + 1);
    strcpy(current->name, name);
    current->type = type;
    return current;
}
SyntaxNode* createNodeFromToken(tToken* token, char* name, int type){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->token = token;
    current->name = (char*)malloc(sizeof(char) * strlen(name) + 1);
    strcpy(current->name, name);
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
    current->right = createNodeFromToken(numberToken, "NumberToken", Node_NumberIntToken);
    current->token = NULL;
    current->type = Node_NumberExpression;
    current->name = "NumberExpression";
    return current;
}

SyntaxNode* identifierExpressionSyntax(tToken* identifier){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->right = createNodeFromToken(identifier, "identifier", Node_IdentifierToken);
    current->type = Node_IdentifierExpression;
    current->name = "IdentifierExpression";
    return current;
}

SyntaxNode* binaryExpressionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->right = right;
    current->statements = NULL;
    current->left = left;
    current->token = operator;
    current->type = Node_BinaryExpression;
    current->name = "BinaryExpression";
    return current;
}
SyntaxNode* binaryConditionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = binaryExpressionSyntax(left, operator, right);
    current->type = Node_BooleanExpression;
    current->name = "BooleanExpression";
    return current;
}
SyntaxNode* unaryExpressionSyntax(tToken* operator, SyntaxNode* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->right = right;
    current->statements = NULL;
    current->left = NULL;
    current->token = operator;
    current->type = Node_UnaryExpression;
    current->name = "UnaryExpression";
    return current;
}

SyntaxNode* parenthezedExpressionSyntax(tToken* left, SyntaxNode* expression, tToken* right){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(left, "OpenParenthesisToken", Node_OpenParenthesisToken);
    current->statements = createNodeList(expression);
    current->token = NULL;
    current->right = createNodeFromToken(right, "CloseParenthesisToken", Node_CloseParenthesisToken);
    current->type = Node_ParenthezedExpression;
    current->name = "ParenthezedExpression";
    return current;
}


SyntaxNode* declExpressionSyntax(tToken* identifier, tToken* equalsToken, SyntaxNode* expression){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(identifier, "identifier", Node_IdentifierToken);
    current->statements = NULL;
    current->token = equalsToken;
    current->right = expression;
    current->type = Node_DeclareExpression;
    current->name = "DeclareExpression";
    return current;
}
SyntaxNode* assignExpressionSyntax(SyntaxNodes* allAssignments){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = NULL;
    current->statements = allAssignments;
    current->token = NULL;
    current->right = NULL;
    current->type = Node_AssignmentExpression;
    current->name = "AssignmentExpression";
    return current;
}
SyntaxNode* blockExpressionSyntax(tToken * left, SyntaxNodes* statements, tToken* right ){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = createNodeFromToken(left, "OpenBlockStatement", Node_OpenBlockStatementToken);
    current->statements = statements;
    current->token = NULL;
    current->right = createNodeFromToken(right, "CloseBlockStatement", Node_CloseBlockStatementToken);
    current->type = Node_BlockExpression;
    current->name = "BlockExpressions";
    return current;
}
SyntaxNode* ifStatementSyntax(tToken * kw, SyntaxNode* condition, SyntaxNode* thenStatements, SyntaxNode* elseStatement){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = condition;
    current->statements = createNodeList(thenStatements);
    current->token = kw;
    current->right = elseStatement;
    current->type = Node_IFExpression;
    current->name = "IfStatement";
    return current;
}

SyntaxNode* elseStatementSyntax(tToken * kw, SyntaxNode* thenStatements ){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    current->left = NULL;
    current->statements = NULL;
    current->token = kw;
    current->right = thenStatements;
    current->type = Node_ElseExpression;
    current->name = "ElseStatement";
    return current;
}
SyntaxNode* forStatementSyntax(tToken* kw, SyntaxNode* definition, SyntaxNode* condition, SyntaxNode* assignExpr, SyntaxNode* body){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    SyntaxNode* forInit = malloc(sizeof(SyntaxNode));
    initSyntaxNode(forInit);
    initSyntaxNode(current);
    forInit->left = definition;
    forInit->statements = createNodeList(assignExpr);
    forInit->right = condition;
    forInit->type = Node_ForInitExpressions;
    forInit->name = "ForInit";
    current->left = forInit;
    current->right = body;
    current->type = Node_ForExpression;
    current->name = "ForStatement";
    return current;
}
SyntaxNode* ParseConditionExpresionSyntax(tTokenizer* tokenizer, tScope* scope){
    SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
    if(expr!= NULL && (expr->type == Node_BooleanExpression || (expr->type == Node_ParenthezedExpression && expr->statements != NULL && expr->statements->node != NULL && expr->statements->node->type == Node_BooleanExpression)))
        return expr;
    if(expr->type == Node_ParenthezedExpression && expr->statements != NULL && expr->statements->node != NULL)
        fprintf(stderr, "Expected: %s \t Given: %s", enumTypeOfNode[Node_BooleanExpression] ,enumTypeOfNode[expr->statements->node->type]);
    else
        fprintf(stderr, "Expected: %s \t Given: %s", enumTypeOfNode[Node_BooleanExpression] ,enumTypeOfNode[expr->type]);
    exit(2);
}

SyntaxNode* ParseDeclarationSyntax(tTokenizer* tokenizer, tScope* scope, tToken* id){
    tToken * declare = Match(tokenizer, tokenType_DECL);
    SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
    return declExpressionSyntax(id, declare, expr);
}
SyntaxNode* ParseAssignSyntax(tTokenizer* tokenizer, tScope* scope, tToken* FirstID){
    tToken *comma = NULL;
    SyntaxNode *idNode = createNodeFromToken(FirstID, "Identifier",Node_IdentifierToken);
    SyntaxNodes* list = createNodeList(createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
    while(tokenizer->outputToken.type != tokenType_ASSIGN && tokenizer->outputToken.type == tokenType_COMMA){
        comma = Match(tokenizer, tokenType_COMMA);
        tToken *newID = Match(tokenizer, tokenType_ID);
        idNode = createNodeFromToken(newID, "Identifier",Node_IdentifierToken);
        addToNodeListEnd(list, createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
    }
    tToken* assign = Match(tokenizer, tokenType_ASSIGN);
    SyntaxNodes * node = list->first;
    while(node != NULL){
        node->node->token = CopyToken(assign);
        node->node->right = ParseExpression(tokenizer, 0, scope);
        if(node->next != NULL)
            comma = Match(tokenizer, tokenType_COMMA);
        node = node->next;
    }
    return assignExpressionSyntax(list);
}
SyntaxNode* PrimaryExpressionSyntax(tTokenizer* tokenizer, tScope* scope){
    // S => (exp)
    if(tokenizer->outputToken.type == tokenType_EOF){
        return NULL;
    }
    if(tokenizer->outputToken.type == tokenType_LBC){
        //createScope(scope);
        tToken* left = Match(tokenizer, tokenType_LBC);
        SyntaxNodes* statements = ParseBlockExpressions(tokenizer, 0, scope);
        tToken* right = Match(tokenizer, tokenType_RBC);

        return blockExpressionSyntax(left, statements, right);
    }
    if(tokenizer->outputToken.type == tokenType_RBC){
        //removeScope(scope);
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
        if (tokenizer->outputToken.type == tokenType_DECL){
            return ParseDeclarationSyntax(tokenizer,scope, identifier);
        }
        if (tokenizer->outputToken.type == tokenType_ASSIGN || tokenizer->outputToken.type == tokenType_COMMA){
            return ParseAssignSyntax(tokenizer, scope, identifier);
        }
        return identifierExpressionSyntax(identifier);
    }
    if(tokenizer->outputToken.type == tokenType_KW){
        tokenizer->eolFlag = EOL_FORBIDEN;
        tToken *kw = Match(tokenizer, tokenType_KW);
        if(strcmp(kw->value, "if") == 0){
            SyntaxNode *condition = ParseConditionExpresionSyntax(tokenizer, scope);
            tokenizer->eolFlag = EOL_OPTIONAL;
            tToken *openBlockToken = Match(tokenizer, tokenType_LBC);

            SyntaxNodes *statements = ParseBlockExpressions(tokenizer, 0, scope);
            tToken *closeBlockToken = Match(tokenizer, tokenType_RBC);
            SyntaxNode *thenStatement = blockExpressionSyntax(openBlockToken, statements, closeBlockToken);
            SyntaxNode * elseSyntax = NULL;
            if(strcmp(tokenizer->outputToken.value, "else") == 0){
                tToken *elsekw = Match(tokenizer, tokenType_KW);
                tokenizer->eolFlag = EOL_OPTIONAL;
                tToken *openBlockTokenElse = Match(tokenizer, tokenType_LBC);
                SyntaxNodes *elseStatements = ParseBlockExpressions(tokenizer, 0, scope);
                tToken *closeBlockTokenElse = Match(tokenizer, tokenType_RBC);
                SyntaxNode *elseStatement = blockExpressionSyntax(openBlockTokenElse, elseStatements, closeBlockTokenElse);
                elseSyntax = elseStatementSyntax(elsekw, elseStatement);
                //addToNodeListEnd(statements, elseSyntax);
            }
            return ifStatementSyntax(kw, condition, thenStatement, elseSyntax);
        }
        if(strcmp(kw->value, "for") == 0){
            //DECLARATION CHECK
            SyntaxNode *definition = NULL;
            SyntaxNode *condition = NULL;
            SyntaxNode *assignExpr = NULL;

            if(tokenizer->outputToken.type != tokenType_SCOMMA) {
                tToken *id = Match(tokenizer, tokenType_ID);
                if (tokenizer->outputToken.type != tokenType_DECL) {
                    fprintf(stderr, "Expected: %s \t got: %s", getEnumString2(tokenType_DECL),
                            getEnumString2(tokenizer->outputToken.type));
                    exit(2);
                }
                tToken *assign = CopyToken(&tokenizer->outputToken);
                getToken(tokenizer);
                SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
                definition = declExpressionSyntax(id, assign, expr); // @TODO CHECK DEFINITION
            }
            tToken* separatorScoma = Match(tokenizer, tokenType_SCOMMA);
            if(tokenizer->outputToken.type != tokenType_SCOMMA) {
                condition = ParseConditionExpresionSyntax(tokenizer, scope); // @TODO CHECK CONDITION
            }
            separatorScoma = Match(tokenizer, tokenType_SCOMMA);
            if(tokenizer->outputToken.type != tokenType_LBC){
                tToken* id = Match(tokenizer, tokenType_ID);
                assignExpr = ParseAssignSyntax(tokenizer, scope, id); // @TODO CHECK Assign
            }
            tokenizer->eolFlag = EOL_OPTIONAL;
            tToken* openBlockToken = Match(tokenizer, tokenType_LBC);
            SyntaxNodes *forStatements = ParseBlockExpressions(tokenizer, 0, scope); // @TODO CHECK BODY
            tToken *closeBlockToken = Match(tokenizer, tokenType_RBC);
            SyntaxNode *forBody = blockExpressionSyntax(openBlockToken, forStatements, closeBlockToken);
            return forStatementSyntax(kw, definition, condition, assignExpr, forBody);
        }

    }

    //S => INT
    if(tokenizer->outputToken.type == tokenType_INT){
        tToken* numberToken = Match(tokenizer, tokenType_INT);
        SyntaxNode *node = numberExpressionSyntax(numberToken);
        return node;
    }
    return NULL;
}
SyntaxNodes* ParseGlobalBlockExpressions (tTokenizer* tokenizer, int parentPriority, tScope* scope){
    SyntaxNodes * list = NULL;
    while (!tokenizer->isEOF) {
        SyntaxNode* expr = ParseExpression(tokenizer, parentPriority, scope);
        if(list == NULL){
            list = createNodeList(expr);
        }else{
            addToNodeListEnd(list, expr);
        }
    }
    return list;
}
SyntaxNodes* ParseBlockExpressions (tTokenizer* tokenizer, int parentPriority, tScope* scope){
    SyntaxNodes * list = NULL;
    while (tokenizer->outputToken.type != tokenType_RBC) {
        SyntaxNode* expr = ParseExpression(tokenizer, parentPriority, scope);
        if(list == NULL){
            list = createNodeList(expr);
        }else{
            addToNodeListEnd(list, expr);
        }
    }
    return list;
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

    while (true){
        int priority = GetBinOperatorPriority(tokenizer->outputToken.type);
        if(priority == 0 || priority <= parentPriority)
            break;
        //Try to Build Binary expression if exists binary statements
        tToken* operator = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);


        SyntaxNode* right = ParseExpression(tokenizer, priority, scope);
        if( operator->type == tokenType_GREATER ||
            operator->type == tokenType_GE ||
            operator->type == tokenType_LESS ||
            operator->type == tokenType_NEQ ||
            operator->type == tokenType_LE)
        {
            left = binaryConditionSyntax(left, operator, right);
        }else{
            left = binaryExpressionSyntax(left, operator, right);
        }
    }
    return left;
}
void printSyntaxTree(SyntaxNode* node, char* indent, bool last) {
    if (node == NULL)
        return;

    char *marker = last ? "└──" : "├──";
    printf("%s%s", indent, marker);
    printf("%s", node->name);
    if (node->token != NULL && node->token->value != NULL) {
        printf(" %s", node->token->value);
    }
    printf("\n");
    char newIndent[1024];
    strcpy(newIndent, indent);
    strcat(newIndent, last ? "   " : "│  ");
    printSyntaxTree(node->left, newIndent, node->statements == NULL && node->right == NULL);
    SyntaxNodes *statement = node->statements != NULL ? node->statements->first : NULL;
    while (statement != NULL){
        printSyntaxTree(statement->node, newIndent, statement->next == NULL && node->right == NULL);
        statement = statement->next;
    }
    printSyntaxTree(node->right, newIndent, true);
}

long eval(tTokenizer* tokenizer, SyntaxNode * root, tScope* scope){
    if(root == NULL){
        return 0;
    }
    if(root->type == Node_BlockExpression) {
        createScope(scope);
        SyntaxNodes *statements = root->statements->first;
        long res = 0;
        while (statements != NULL) {
            res = eval(tokenizer, statements->node, scope);
            statements = statements->next;
        }
        removeLastLocalScope(scope);

        return res;
    }
    if(root->type == Node_NumberExpression){
        char* end;
        long result = strtol (root->right->token->value, &end, 10);
        printf("\t%s\tRES >>> \t%ld\n", root->name,result);
        return result;
    }
    if(root->type == Node_IdentifierExpression){
        tScopeItem *currentScope = scope->topLocal;
        tHashItem *item;
        do {
            item = getHashItem(currentScope->table, root->right->token->value);
            if(item == NULL){
                currentScope = currentScope->next;
            }
        }while(item == NULL && currentScope != NULL);

        if(item == NULL){
            fprintf(stderr, "Identifier %s was not declared!", root->right->token->value);
            exit(3);
        }
        char* end;
        long result = strtol (item->value,&end,10);
        printf("\t%s\tRES >>> \t%ld\n", root->name,result);
        return result;
    }
    if(root->type == Node_AssignmentExpression){
        tHashItem *item = getHashItem(scope->topLocal->table, root->left->token->value);
        if(item == NULL){
            if(root->token->type == tokenType_DECL){
                char value[1024];
                long res = eval(tokenizer, root->right, scope);
                sprintf(value, "%ld", res);
                if(addDataToHT(scope->topLocal->table, root->left->token->value, value, true) == 1){
                    fprintf(stderr, "ERRTABLE\n");
                }
                printf("\t%s\tRES >>> \t%ld\n", root->name,res);
                return res;
            }else if (root->token->type == tokenType_ASSIGN){
                tScopeItem *currentScope = scope->topLocal->next;
                while(item == NULL && currentScope != NULL){
                    item = getHashItem(currentScope->table, root->left->token->value);
                    if(item == NULL){
                        currentScope = currentScope->next;
                    }
                }
                if(currentScope == NULL){
                    fprintf(stderr, "Identifier %s was not declared! CannotAssign\n", root->left->token->value);
                    exit(3);
                }
                char value[1024];
                long res = eval(tokenizer, root->right, scope);
                sprintf(value, "%ld", res);
                strcpy(item->value, value);
                printf("\t%s\tRES >>> \t%ld\n", root->name,res);
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
            printf("\t%s\tRES >>> \t%ld\n", root->name,res);
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
        return eval(tokenizer, root->statements->node, scope);
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
    //deleteSyntaxTree(node->statements);
    deleteSyntaxTree(node->right);
    free(node);

}
