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
    destructHashTable(tmp->table);
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
        case tokenType_EOL: return "EOL";
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
    nextListItem->first = list;
    list->last->next = nextListItem;
    list->last = nextListItem;
    return 0;
}
int destroyNodeList(SyntaxNodes* list){
    if(list == NULL){
        return 0;
    }

    SyntaxNodes * current = list->first;

    while(current != NULL){
        SyntaxNodes*tmp = current;
        SyntaxNode *tmpNode = current->node;
        if(tmpNode != NULL){
            deleteSyntaxTree(tmpNode);
            current->node = NULL;
            tmpNode = NULL;
        }
        current->first = NULL;
        current->last = NULL;
        current = current->next;
        free(tmp);
    }

    return 0;
}

SyntaxNode* createNode(SyntaxNode* left, SyntaxNodes* statements, SyntaxNode* right, tToken* token, const char* name, int type){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    if(current != NULL){
        initSyntaxNode(current);
        current->left = left;
        current->statements = statements;
        current->right = right;
        current->token = token;
        if(name != NULL){
            current->name = (char*)malloc(sizeof(char) * strlen(name) + 1);
            strcpy(current->name, name);
        }
        current->type = type;
    }
    return current;
}

SyntaxNode* createNodeFromToken(tToken* token, char* name, int type){
    SyntaxNode* current = malloc(sizeof(SyntaxNode));
    initSyntaxNode(current);
    current->token = token;
    if(name != NULL){
        current->name = (char*)malloc(sizeof(char) * strlen(name) + 1);
        strcpy(current->name, name);
    }
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
SyntaxNodes* CopyNodeList(SyntaxNodes* nodes){
    if(nodes == NULL)
        return NULL;
    SyntaxNodes* copyList = createNodeList(nodes->first->node);
    SyntaxNodes* current = nodes->first;
    while(current != NULL){
        addToNodeListEnd(copyList, CopyNode(current->node));
        current = current->next;
    }
    return copyList;
}
SyntaxNode* CopyNode(SyntaxNode* node){
    if(node == NULL)
        return NULL;
    char* copyName = NULL;
    if(node->name != NULL) {
        copyName = (char *) malloc(sizeof(char) * strlen(node->name) + 1);
        if(copyName != NULL)
            strcpy(copyName, node->name);
    }
    return createNode(CopyNode(node->left), CopyNodeList(node->statements),CopyNode(node->right), CopyToken(node->token), copyName, node->type);
}

tToken* Match(tTokenizer* tokenizer, int type, bool copy){
    tToken* current = NULL;
    if(tokenizer->outputToken.type == type){
        if(copy)
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
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            createNodeFromToken(numberToken, "NumberIntToken", Node_NumberIntToken),
            NULL,
            "NumberIntExpression",
            Node_NumberIntExpression
    );
    return current;
}
SyntaxNode* doubleExpressionSyntax(tToken* doubleToken){
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            createNodeFromToken(doubleToken, "NumberDoubleToken", Node_NumberDoubleToken),
            NULL,
            "NumberDoubleExpression",
            Node_NumberDoubleExpression
    );
    return current;
}
SyntaxNode* stringExpressionSyntax(tToken* stringToken){
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            createNodeFromToken(stringToken, "StringToken", Node_StringToken),
            NULL,
            "StringExpression",
            Node_StringExpression
    );
    return current;
}

SyntaxNode* identifierExpressionSyntax(tToken* identifier){
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            createNodeFromToken(identifier, "identifier", Node_IdentifierToken),
            NULL,
            "IdentifierExpression",
            Node_IdentifierExpression
    );
    return current;
}

SyntaxNode* binaryExpressionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = createNode(
            left,
            NULL,
            right,
            operator,
            "BinaryExpression",
            Node_BinaryExpression
    );
    return current;
}
SyntaxNode* binaryConditionSyntax(SyntaxNode* left, tToken* operator, SyntaxNode* right){
    SyntaxNode* current = createNode(
            left,
            NULL,
            right,
            operator,
            "BooleanExpression",
            Node_BooleanExpression
    );
    return current;
}
SyntaxNode* unaryExpressionSyntax(tToken* operator, SyntaxNode* right){
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            right,
            operator,
            "UnaryExpression",
            Node_UnaryExpression
    );
    return current;
}

SyntaxNode* parenthezedExpressionSyntax(tToken* left, SyntaxNode* expression, tToken* right){
    SyntaxNode* current = createNode(
            createNodeFromToken(left, "OpenParenthesisToken", Node_OpenParenthesisToken),
            createNodeList(expression),
            createNodeFromToken(right, "CloseParenthesisToken", Node_CloseParenthesisToken),
            NULL,
            "ParenthezedExpression",
            Node_ParenthezedExpression
    );
    return current;
}


SyntaxNode* declExpressionSyntax(tToken* identifier, tToken* equalsToken, SyntaxNode* expression){
    SyntaxNode* current = createNode(
            createNodeFromToken(identifier, "identifier", Node_IdentifierToken),
            NULL,
            expression,
            equalsToken,
            "DeclareExpression",
            Node_DeclareExpression
    );
    return current;
}
SyntaxNode* assignExpressionSyntax(SyntaxNodes* assignTo, tToken* assign,SyntaxNodes* assignValues){
    SyntaxNode* assignToAll = createNode(
            NULL,
            assignTo,
            NULL,
            NULL,
            "AssignmentToExpression",
            Node_AssignmentToExpression
    );
    SyntaxNode* assignValuesAll = createNode(
            NULL,
            assignValues,
            NULL,
            NULL,
            "AssignmentValuesExpression",
            Node_AssignmentValuesExpression
    );

    SyntaxNode* current = createNode(
            assignToAll,
            NULL,
            assignValuesAll,
            assign,
            "AssignmentExpression",
            Node_AssignmentExpression
    );
    return current;
}

SyntaxNode* blockExpressionSyntax(tToken * left, SyntaxNodes* statements, tToken* right ){
    SyntaxNode* current = createNode(
            createNodeFromToken(left, "OpenBlockStatement", Node_OpenBlockStatementToken),
            statements,
            createNodeFromToken(right, "CloseBlockStatement", Node_CloseBlockStatementToken),
            NULL,
            "BlockExpressions",
            Node_BlockExpression
    );
    return current;
}
SyntaxNode* ifStatementSyntax(tToken * kw, SyntaxNode* condition, SyntaxNode* thenStatements, SyntaxNode* elseStatement){
    SyntaxNode* current = createNode(
            condition,
            createNodeList(thenStatements),
            elseStatement,
            kw,
            "IfStatement",
            Node_IFExpression
    );
    return current;
}

SyntaxNode* elseStatementSyntax(tToken * kw, SyntaxNode* thenStatements ){
    SyntaxNode* current = createNode(
            NULL,
            NULL,
            thenStatements,
            kw,
            "ElseStatement",
            Node_ElseExpression
    );
    return current;
}
SyntaxNode* forStatementSyntax(tToken* kw, SyntaxNode* definition, SyntaxNode* condition, SyntaxNode* assignExpr, SyntaxNode* body){
    SyntaxNode* forInit = createNode(
            definition,
            createNodeList(assignExpr),
            condition,
            CopyToken(kw),
            "ForInit",
            Node_ForInitExpressions
    );
    SyntaxNode* current = createNode(
            forInit,
            NULL,
            body,
            kw,
            "ForStatement",
            Node_ForExpression
    );
    return current;
}

SyntaxNode* functStatementSyntax(tToken *kw, tToken* id, SyntaxNodes* params, SyntaxNodes* returnTypes, SyntaxNode* body){
    SyntaxNode* requiredHeader = createNode(
            createNodeFromToken(id, "FunctionIdentifierToken", Node_IdentifierToken),
            params,
            NULL,
            NULL,
            "FunctionRequiredInformation",
            Node_FunctionInitExpression
    );
    SyntaxNode* current = createNode(
            requiredHeader,
            returnTypes,
            body,
            kw,
            "FunctionStatement",
            Node_FunctionExpression
    );
    return current;
}

SyntaxNode* returnExpressionSyntax(tToken* kw, SyntaxNodes* returnValues){
    SyntaxNode* current = createNode(
            NULL,
            returnValues,
            NULL,
            kw,
            "ReturnStatement",
            Node_ReturnExpression
    );
    return current;
}
SyntaxNode* functionCallExpressionSyntax(tToken* id, SyntaxNodes* params){
    SyntaxNode* current = createNode(
            NULL,
            params,
            NULL,
            id,
            "FunctionCallExpression",
            Node_FunctionCallExpression
    );
    return current;
}

SyntaxNode* ParseConditionExpresionSyntax(tTokenizer* tokenizer, tScope* scope){
    SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
    if(expr!= NULL && (expr->type == Node_BooleanExpression || (expr->type == Node_ParenthezedExpression && expr->statements != NULL && expr->statements->node != NULL && expr->statements->node->type == Node_BooleanExpression)))
        return expr;
    if(expr->type == Node_ParenthezedExpression && expr->statements != NULL && expr->statements->node != NULL)
        fprintf(stderr, "Expected: %s \t Given: %s\n", enumTypeOfNode[Node_BooleanExpression] ,enumTypeOfNode[expr->statements->node->type]);
    else
        fprintf(stderr, "Expected: %s \t Given: %s\n", enumTypeOfNode[Node_BooleanExpression] ,enumTypeOfNode[expr->type]);
    exit(2);
}

SyntaxNode* ParseDeclarationSyntax(tTokenizer* tokenizer, tScope* scope, tToken* id){
    tToken * declare = Match(tokenizer, tokenType_DECL, true);
    SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
    if(expr == NULL){
        fprintf(stderr, "Expected expression after declaration!\n");
        exit(2);
    }
    return declExpressionSyntax(id, declare, expr);
}
SyntaxNode* ParseFunctionCallingSyntax(tTokenizer* tokenizer, tScope* scope, tToken* id){
    tToken * openBracket = Match(tokenizer, tokenType_LBN, false);

    /*while(tokenizer->outputToken.type == tokenType_EOL){
        getToken(tokenizer);
    }*/
    tToken* comma = NULL;
    SyntaxNode* expr = NULL;
    SyntaxNodes* params = NULL;
    while (tokenizer->outputToken.type != tokenType_RBN){
        expr = NULL;
        if(tokenizer->outputToken.type == tokenType_COMMA) {
            comma = Match(tokenizer, tokenType_COMMA, false);
            /*while(tokenizer->outputToken.type == tokenType_EOL){
                getToken(tokenizer);
            }*/
        }
        expr = ParseExpression(tokenizer, 0, scope);

        if(expr!= NULL){
            if(expr->type == Node_FunctionCallParameters){
                SyntaxNodes* newExprs = expr->statements->first;
                while (newExprs != NULL) {
                    SyntaxNode *newNode = createNode(newExprs->node->left, CopyNodeList(newExprs->node->statements),
                                                     newExprs->node->right, newExprs->node->token,
                                                     "IdentifierExpression", Node_IdentifierExpression);
                    if(newExprs->node != NULL){
                        if(newExprs->node->name != NULL)
                            free(newExprs->node->name);
                        free(newExprs->node);
                    }
                    if (params == NULL) {
                        params = createNodeList(newNode);
                    } else {
                        addToNodeListEnd(params, newNode);
                    }
                    SyntaxNodes *tmpNodes = newExprs;
                    newExprs->first = NULL;
                    newExprs->last = NULL;
                    newExprs = newExprs->next;
                    free(tmpNodes);
                }
                if(expr->name != NULL)
                    free(expr->name);
                free(expr);
                expr = NULL;
            }else {

                if (params == NULL) {
                    params = createNodeList(expr);
                } else {
                    addToNodeListEnd(params, expr);
                }
            }

        }

    }
    tToken* CloseBracket = Match(tokenizer, tokenType_RBN, false);
    //tToken* EOL = Match(tokenizer, tokenType_EOL, false);
    return functionCallExpressionSyntax(id, params);
}
SyntaxNode* ParseAssignSyntax(tTokenizer* tokenizer, tScope* scope, tToken* FirstID){
    tToken *comma = NULL;
    SyntaxNode *idNode = createNodeFromToken(FirstID, "Identifier",Node_IdentifierToken);
    SyntaxNodes* list = createNodeList(createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
    while(tokenizer->outputToken.type != tokenType_ASSIGN && tokenizer->outputToken.type == tokenType_COMMA){
        comma = Match(tokenizer, tokenType_COMMA, false);
        tToken *newID = Match(tokenizer, tokenType_ID, true);
        idNode = createNodeFromToken(newID, "Identifier",Node_IdentifierToken);
        addToNodeListEnd(list, createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
    }
    tToken* assign = Match(tokenizer, tokenType_ASSIGN, true);
    SyntaxNodes * node = list->first;
    SyntaxNodes* assignValues = NULL;
    SyntaxNode* prevNode = NULL;
    bool prevNodeFuncType = false;
    while(node != NULL){
        if( (prevNode != NULL && !prevNodeFuncType) || tokenizer->outputToken.type == tokenType_COMMA){
            comma = Match(tokenizer, tokenType_COMMA, false);
            if(assignValues == NULL){
                assignValues = createNodeList(ParseExpression(tokenizer, 0, scope));
            }else{
                addToNodeListEnd(assignValues, ParseExpression(tokenizer, 0, scope));
            }
        }else if (prevNode == NULL){
            if(assignValues == NULL){
                assignValues = createNodeList(ParseExpression(tokenizer, 0, scope));
            }else{
                addToNodeListEnd(assignValues, ParseExpression(tokenizer, 0, scope));
            }
        }else if(prevNodeFuncType || node->next == NULL){
            break;
        }else{
            fprintf(stderr, "Assignment went wrong!\n");
            exit(2);
        }
        prevNode = assignValues->last->node;
        prevNodeFuncType = assignValues->last->node->type == Node_FunctionCallExpression ? true: prevNodeFuncType;
        node = node->next;
    }
    return assignExpressionSyntax(list, assign,assignValues);
}
SyntaxNode* PrimaryExpressionSyntax(tTokenizer* tokenizer, tScope* scope){
    // S => (exp)
    static int functionReturnParams = -1;
    static bool parsingReturn = false;
    if(tokenizer->outputToken.type == tokenType_EOF){
        fprintf(stderr, "Unexpected EOF!\n");
        exit(2);
    }
    if(tokenizer->outputToken.type == tokenType_LBC){
        //createScope(scope);
        /*tToken* left = Match(tokenizer, tokenType_LBC);
        SyntaxNodes* statements = ParseBlockExpressions(tokenizer, 0, scope);
        tToken* right = Match(tokenizer, tokenType_RBC);

        return blockExpressionSyntax(left, statements, right);
         */
        fprintf(stderr, "Unexpected start of the BLOCK!\n");
        exit(2);
    }
    if(tokenizer->outputToken.type == tokenType_RBC){
        fprintf(stderr, "Unexpected end of the BLOCK!\n");
        exit(2);
    }
    if(tokenizer->outputToken.type == tokenType_COMMA){
        fprintf(stderr, "Unexpected comma!\n");
        exit(2);
    }
    if(tokenizer->outputToken.type == tokenType_LBN){
        tToken *left = Match(tokenizer, tokenType_LBN, true);
        while (tokenizer->outputToken.type == tokenType_EOL)
            getToken(tokenizer);
        SyntaxNode *expression = ParseExpression(tokenizer, 0, scope);
        tToken *right = Match(tokenizer, tokenType_RBN, true);
        return parenthezedExpressionSyntax(left, expression, right);
    }

    if(tokenizer->outputToken.type == tokenType_ID){
        bool isSpace = tokenizer->actualChar == ' ';
        tToken *identifier = Match(tokenizer, tokenType_ID, true);
        if((tokenizer->outputToken.type == tokenType_ASSIGN || tokenizer->outputToken.type == tokenType_DECL) && parsingReturn){
            fprintf(stderr, "Expected comma or expression, not Assignment or Declaration!\n");
            exit(2);
        }
        if (tokenizer->outputToken.type == tokenType_DECL && !parsingReturn){
            return ParseDeclarationSyntax(tokenizer,scope, identifier);
        }
        if ((tokenizer->outputToken.type == tokenType_ASSIGN || tokenizer->outputToken.type == tokenType_COMMA) && !parsingReturn){
            tToken *comma = NULL;
            SyntaxNode *idNode = createNodeFromToken(identifier, "Identifier",Node_IdentifierToken);
            SyntaxNodes* list = createNodeList(createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
            while(tokenizer->outputToken.type != tokenType_ASSIGN && tokenizer->outputToken.type == tokenType_COMMA) {
                comma = Match(tokenizer, tokenType_COMMA, false);
                if(tokenizer->outputToken.type != tokenType_ID){
                    return createNode(NULL, list, NULL, NULL, "FUNCTION CALL PARAMS", Node_FunctionCallParameters);
                }
                tToken *newID = Match(tokenizer, tokenType_ID, true);
                idNode = createNodeFromToken(newID, "Identifier",Node_IdentifierToken);
                addToNodeListEnd(list, createNode(idNode, NULL, NULL, NULL, "IdentifierAssignmentONE", Node_AssignmentExpression));
            }
            if(tokenizer->outputToken.type != tokenType_ASSIGN){
                return createNode(NULL, list, NULL, NULL, "FUNCTION CALL PARAMS", Node_FunctionCallParameters);
            }
            tToken* assign = Match(tokenizer, tokenType_ASSIGN, true);
            while (tokenizer->outputToken.type == tokenType_EOL)
                getToken(tokenizer);

            SyntaxNodes * node = list->first;
            SyntaxNodes* assignValues = NULL;
            SyntaxNode* prevNode = NULL;
            bool prevNodeFuncType = false;
            while(node != NULL){
                if( (prevNode != NULL && !prevNodeFuncType) || (prevNode != NULL && tokenizer->outputToken.type == tokenType_COMMA)){
                    comma = Match(tokenizer, tokenType_COMMA, false);
                    while (tokenizer->outputToken.type == tokenType_EOL)
                        getToken(tokenizer);
                    SyntaxNode* expr = ParseExpression(tokenizer, 0, scope);
                    if(expr != NULL) {
                        if (expr->type == Node_IdentifierToken ||
                            expr->type == Node_ParenthezedExpression ||
                            expr->type == Node_BinaryExpression ||
                            expr->type == Node_IdentifierExpression ||
                            expr->type == Node_UnaryExpression ||
                            expr->type == Node_NumberIntToken ||
                            expr->type == Node_NumberIntExpression ||
                            expr->type == Node_NumberDoubleExpression ||
                            expr->type == Node_NumberDoubleToken ||
                            expr->type == Node_StringExpression ||
                            expr->type == Node_StringToken
                                ) {
                            if (assignValues == NULL) {
                                assignValues = createNodeList(expr);
                            } else {
                                addToNodeListEnd(assignValues, expr);
                            }
                        } else {
                            fprintf(stderr, "Assignment went wrong!\n");
                            exit(2);
                        }
                    }
                }else if (prevNode == NULL){
                    if(assignValues == NULL){
                        SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
                        if(expr!=NULL){
                            assignValues = createNodeList(expr);
                        }else{
                            fprintf(stderr, "Expected expr, given EOL!\n");
                            exit(2);
                        }
                    }else{
                        addToNodeListEnd(assignValues, ParseExpression(tokenizer, 0, scope));
                    }
                }else if(prevNodeFuncType || node->next == NULL){
                    break;
                }else{
                    fprintf(stderr, "Assignment went wrong!\n");
                    exit(2);
                }
                prevNode = assignValues->last->node;
                prevNodeFuncType = assignValues->last->node->type == Node_FunctionCallExpression ? true: prevNodeFuncType;
                node = node->next;
            }
            return assignExpressionSyntax(list, assign,assignValues);
        }
        if(tokenizer->outputToken.type == tokenType_LBN){
            if(isSpace){
                fprintf(stderr, "Function call should not have ' ' after identifier!\n");
                exit(2);
            }
            return ParseFunctionCallingSyntax(tokenizer, scope, identifier);
        }
        return identifierExpressionSyntax(identifier);
    }
    if(tokenizer->outputToken.type == tokenType_KW){

        tToken *kw = Match(tokenizer, tokenType_KW, true);
        if(strcmp(kw->value, "if") == 0){
            SyntaxNode *condition = ParseConditionExpresionSyntax(tokenizer, scope);
            tToken *openBlockToken = Match(tokenizer, tokenType_LBC, true);
            tToken *EOL = Match(tokenizer, tokenType_EOL, false);
            SyntaxNodes *statements = ParseBlockExpressions(tokenizer, 0, scope);
            tToken *closeBlockToken = Match(tokenizer, tokenType_RBC, true);
            SyntaxNode *thenStatement = blockExpressionSyntax(openBlockToken, statements, closeBlockToken);
            SyntaxNode * elseSyntax = NULL;
            tToken *elsekw = Match(tokenizer, tokenType_KW, true);
            if(strcmp(elsekw->value, "else") != 0){
                fprintf(stderr, "ELSE required!\n");
                exit(2);
            }
            tToken *openBlockTokenElse = Match(tokenizer, tokenType_LBC, true);
            EOL = Match(tokenizer, tokenType_EOL, false);
            SyntaxNodes *elseStatements = ParseBlockExpressions(tokenizer, 0, scope);
            tToken *closeBlockTokenElse = Match(tokenizer, tokenType_RBC, true);
            EOL = Match(tokenizer, tokenType_EOL, false);
            SyntaxNode *elseStatement = blockExpressionSyntax(openBlockTokenElse, elseStatements, closeBlockTokenElse);
            elseSyntax = elseStatementSyntax(elsekw, elseStatement);
            //addToNodeListEnd(statements, elseSyntax);

            return ifStatementSyntax(kw, condition, thenStatement, elseSyntax);
        }
        if(strcmp(kw->value, "for") == 0){
            //DECLARATION CHECK
            SyntaxNode *definition = NULL;
            SyntaxNode *condition = NULL;
            SyntaxNode *assignExpr = NULL;

            if(tokenizer->outputToken.type != tokenType_SCOMMA) {
                tToken *id = Match(tokenizer, tokenType_ID, true);
                if (tokenizer->outputToken.type != tokenType_DECL) {
                    fprintf(stderr, "Expected: %s \t got: %s\n", getEnumString2(tokenType_DECL),
                            getEnumString2(tokenizer->outputToken.type));
                    exit(2);
                }
                tToken *assign = CopyToken(&tokenizer->outputToken);
                getToken(tokenizer);
                SyntaxNode *expr = ParseExpression(tokenizer, 0, scope);
                definition = declExpressionSyntax(id, assign, expr);
            }
            tToken* separatorScoma = Match(tokenizer, tokenType_SCOMMA, false);
            if(tokenizer->outputToken.type != tokenType_SCOMMA) {
                condition = ParseConditionExpresionSyntax(tokenizer, scope);
            }
            separatorScoma = Match(tokenizer, tokenType_SCOMMA, false);
            if(tokenizer->outputToken.type != tokenType_LBC){
                tToken* id = Match(tokenizer, tokenType_ID, true);
                assignExpr = ParseAssignSyntax(tokenizer, scope, id);
            }
            tToken* openBlockToken = Match(tokenizer, tokenType_LBC, true);
            tToken *EOL = Match(tokenizer, tokenType_EOL, false);
            SyntaxNodes *forStatements = ParseBlockExpressions(tokenizer, 0, scope);
            tToken *closeBlockToken = Match(tokenizer, tokenType_RBC, true);
            SyntaxNode *forBody = blockExpressionSyntax(openBlockToken, forStatements, closeBlockToken);
            return forStatementSyntax(kw, definition, condition, assignExpr, forBody);
        }
        if(strcmp(kw->value, "func") == 0){
            tToken* functionNameToken = Match(tokenizer, tokenType_ID, true);
            tToken* OpenBracket = Match(tokenizer, tokenType_LBN, false);
            SyntaxNodes* params = NULL;
            tToken* comma = NULL;
            int returnParamsCount = 0;
            while(tokenizer->outputToken.type != tokenType_RBN){
                tToken* paramID = Match(tokenizer, tokenType_ID, true);
                tToken* paramType = Match(tokenizer, tokenType_KW, true);
                if(tokenizer->outputToken.type == tokenType_COMMA){
                    comma = Match(tokenizer, tokenType_COMMA, false);
                    /*while (tokenizer->outputToken.type == tokenType_EOL){
                        getToken(tokenizer);
                    }
                     */

                }
                else if(tokenizer->outputToken.type != tokenType_COMMA && tokenizer->outputToken.type != tokenType_RBN){
                    fprintf(stderr, "Expected: COMMA or CLOSING BRACKET!\n");
                    exit(2);
                }
                SyntaxNode* oneParam = createNode(
                        createNodeFromToken(paramID, "ParameterIDToken", Node_ParamIdentifierToken),
                        NULL,
                        createNodeFromToken(paramType, "ParameterTypeToken", Node_ParamTypeToken),
                        NULL,
                        "OneFunctionParameter",
                        Node_FunctionParameter
                );
                if(params == NULL){
                    params = createNodeList(oneParam);
                }else{
                    addToNodeListEnd(params, oneParam);
                }
            }
            tToken* CloseBracket = Match(tokenizer, tokenType_RBN, false);
            SyntaxNodes* returnTypes = NULL;
            if(tokenizer->outputToken.type == tokenType_LBN){
                OpenBracket = Match(tokenizer, tokenType_LBN, false);
                while (tokenizer->outputToken.type != tokenType_RBN){
                    tToken* returnType = Match(tokenizer, tokenType_KW, true);
                    if(tokenizer->outputToken.type == tokenType_COMMA){
                        comma = Match(tokenizer, tokenType_COMMA, false);
                    }
                    else if(tokenizer->outputToken.type != tokenType_COMMA && tokenizer->outputToken.type != tokenType_RBN){
                        fprintf(stderr, "Expected: COMMA or CLOSING BRACKET!\n");
                        exit(2);
                    }
                    if(returnTypes == NULL){
                        returnTypes = createNodeList(createNodeFromToken(returnType, "ReturnType", Node_KWTypeToken));
                    }else{
                        addToNodeListEnd(returnTypes, createNodeFromToken(returnType, "ReturnType", Node_KWTypeToken));
                    }
                    ++returnParamsCount;
                }
                CloseBracket = Match(tokenizer, tokenType_RBN, false);
            }
            tToken* openBlockToken = Match(tokenizer, tokenType_LBC, true);
            tToken* EOL = Match(tokenizer, tokenType_EOL, false);
            functionReturnParams = returnParamsCount;
            SyntaxNodes* funcStatements = ParseBlockExpressions(tokenizer, 0, scope);
            tToken *closeBlockToken = Match(tokenizer, tokenType_RBC, true);
            functionReturnParams = -1;
            SyntaxNode* funcBody = blockExpressionSyntax(openBlockToken, funcStatements, closeBlockToken);
            return functStatementSyntax(kw, functionNameToken, params, returnTypes, funcBody);
        }
        if(strcmp(kw->value, "return") == 0){
            parsingReturn = true;
            SyntaxNodes* returnValues = NULL;
            SyntaxNode* expr = NULL;
            tToken* comma = NULL;
            int params = 0;
            while(params++ < functionReturnParams){
                if(params != 1 && params <= functionReturnParams)
                    comma = Match(tokenizer, tokenType_COMMA, false);
                expr = ParseExpression(tokenizer, 0, scope);
                if(expr == NULL){
                    fprintf(stderr, "return expect %d expressions. \t Given: %d\n", functionReturnParams, params-1);
                    exit(2);
                }
                if(returnValues == NULL){
                    returnValues = createNodeList(expr);
                }else{
                    addToNodeListEnd(returnValues, expr);
                }
            }
            if(tokenizer->outputToken.type == tokenType_COMMA){
                fprintf(stderr,"Too many return values!\n");
                exit(2);
            }
            parsingReturn = false;
            return returnExpressionSyntax(kw, returnValues);
        }
        if(strcmp(kw->value, "package") == 0){
            fprintf(stderr, "Unexpected 'package'\n");
            exit(2);
        }
        if(strcmp(kw->value, "else") == 0){
            fprintf(stderr, "Unexpected 'else'\n");
            exit(2);
        }
        /*
        if(strcmp(kw->value, "package") == 0){
            fprintf(stderr, "Unexpected 'package'");
            exit(2);
        }*/

    }

    //S => INT
    if(tokenizer->outputToken.type == tokenType_INT){
        tToken* numberToken = Match(tokenizer, tokenType_INT, true);
        SyntaxNode *node = numberExpressionSyntax(numberToken);
        return node;
    }
    if(tokenizer->outputToken.type == tokenType_DOUBLE){
        tToken* doubleToken = Match(tokenizer, tokenType_DOUBLE, true);
        SyntaxNode *node = doubleExpressionSyntax(doubleToken);
        return node;
    }

    if(tokenizer->outputToken.type == tokenType_STRING){
        tToken* stringToken = Match(tokenizer, tokenType_STRING, true);
        SyntaxNode *node = stringExpressionSyntax(stringToken);
        return node;
    }
    if( tokenizer->outputToken.type == tokenType_GREATER ||
        tokenizer->outputToken.type == tokenType_GE ||
        tokenizer->outputToken.type == tokenType_LESS ||
        tokenizer->outputToken.type == tokenType_NEQ ||
        tokenizer->outputToken.type == tokenType_EQ ||
        tokenizer->outputToken.type == tokenType_LE)
    {
        fprintf(stderr,"Unexpected comparison token!\n");
        exit(2);
    }
    if( tokenizer->outputToken.type == tokenType_ASSIGN ||
        tokenizer->outputToken.type == tokenType_DECL
            ) {
        fprintf(stderr,"Expected EOL or continue expression. Not Declaration or Assignment!\n");
        exit(2);
    }
    return NULL;
}
SyntaxNodes* ParseGlobalBlockExpressions (tTokenizer* tokenizer, int parentPriority, tScope* scope){
    SyntaxNodes * list = NULL;
    while (!tokenizer->isEOF) {
        SyntaxNode* expr = ParseExpression(tokenizer, parentPriority, scope);
        if(tokenizer->outputToken.type == tokenType_EOL){
            getToken(tokenizer);
        }
        if(expr != NULL) {
            if (list == NULL) {
                list = createNodeList(expr);
            } else {
                addToNodeListEnd(list, expr);
            }
        }
    }
    return list;
}
SyntaxNodes* ParseBlockExpressions (tTokenizer* tokenizer, int parentPriority, tScope* scope){
    SyntaxNodes * list = NULL;
    while (tokenizer->outputToken.type != tokenType_RBC) {
        SyntaxNode* expr = ParseExpression(tokenizer, parentPriority, scope);
        if(tokenizer->outputToken.type == tokenType_EOL){
            getToken(tokenizer);
        }
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
    if (tokenizer->outputToken.type == tokenType_EOL)
        return NULL;
    if(tokenizer->errorCode == 1){
        fprintf(stderr, "Lex chyba!\n");
        exit(1);
    }if(tokenizer->errorCode == 99){
        fprintf(stderr, "MALOC!!!\n");
        exit(99);
    }
    if(tokenizer->outputToken.type == tokenType_SCOMMA){
        fprintf(stderr, "Unexpected ';'\n");
        exit(2);
    }
    SyntaxNode* left;
    int unaryPriority = GetUnOperatorPriority(tokenizer->outputToken.type);
    //S => -exp | +exp
    if(unaryPriority != 0 && unaryPriority >= parentPriority){
        tToken* operator = Match(tokenizer, tokenizer->outputToken.type, true);
        SyntaxNode* operand = ParseExpression(tokenizer, unaryPriority, scope);
        left = unaryExpressionSyntax(operator, operand);
    }else{
        left = PrimaryExpressionSyntax(tokenizer, scope);
        if( left->type == Node_IFExpression ||
            left->type == Node_ForExpression
                )
            return left;
    }
    while ( tokenizer->outputToken.type != tokenType_EOF &&
            tokenizer->outputToken.type != tokenType_EOL &&
            tokenizer->outputToken.type != tokenType_SCOMMA &&
            tokenizer->outputToken.type != tokenType_RBN &&
            tokenizer->outputToken.type != tokenType_LBC &&
            tokenizer->outputToken.type != tokenType_COMMA
            )
    {
        int priority = GetBinOperatorPriority(tokenizer->outputToken.type);
        if(priority == 0 && (left->type == Node_StringExpression || left->type == Node_FunctionCallParameters))  {
            break;
        }
        if(priority == 0 || priority <= parentPriority) {
            if(priority == 0){
                fprintf(stderr, "Unexpedted token!!! --- \t%s\n", getEnumString2(tokenizer->outputToken.type));
                exit(2);
            }
            break;
        }
        //Try to Build Binary expression if exists binary statements
        tToken* operator = CopyToken(&tokenizer->outputToken);
        getToken(tokenizer);
        while (tokenizer->outputToken.type == tokenType_EOL)
            getToken(tokenizer);

        SyntaxNode* right = ParseExpression(tokenizer, priority, scope);
        if(right == NULL){
            fprintf(stderr, "Unexpected EOL!\n");
            exit(2);
        }
        if( right->type == Node_DeclareExpression ||
            right->type == Node_AssignmentExpression
                ){
            fprintf(stderr, "Unexpected Declare or Assign Expression\n");
            exit(2);
        }
        if( operator->type == tokenType_GREATER ||
            operator->type == tokenType_GE ||
            operator->type == tokenType_LESS ||
            operator->type == tokenType_NEQ ||
            operator->type == tokenType_EQ ||
            operator->type == tokenType_LE)
        {
            left = binaryConditionSyntax(left, operator, right);
            break;
        }else{
            left = binaryExpressionSyntax(left, operator, right);
        }
    }
    return left;
}

SyntaxNode* getPackage(tTokenizer* tokenizer){
    /* if(tokenizer->outputToken.type != tokenType_KW){
         return NULL;
     }
     */
    while (tokenizer->outputToken.type == tokenType_EOL)
        getToken(tokenizer);

    tToken* pkKW = Match(tokenizer, tokenType_KW, true);
    if(strcmp(pkKW->value, "package") != 0){
        fprintf(stderr, "Expected 'package'. Given: %s\n", pkKW->value);
        exit(2);
    }
    tToken* idofPk = Match(tokenizer, tokenType_ID, true);
    tToken* EOL = Match(tokenizer, tokenType_EOL, false);
    if(strcmp(idofPk->value, "main") != 0){
        fprintf(stderr, "Expected 'main'. Given: %s\n", idofPk->value);
        exit(2);
    }
    return createNode(createNodeFromToken(idofPk, "PackageID", Node_PackageNameToken), NULL, NULL, pkKW, "Package", Node_PackageExpression);
}
void printSyntaxTree(SyntaxNode* node, char* indent, bool last) {
    if (node == NULL)
        return;

    char *marker = last ? "└──" : "├──";
    printf("%s%s", indent, marker);
    if(node->name != NULL)
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
    if(root->type == Node_NumberIntExpression){
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
            fprintf(stderr, "Identifier %s was not declared!\n", root->right->token->value);
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
//    printSyntaxTree(node, "", true);
    if(node->token != NULL){
        if(node->token->value != NULL){
            free(node->token->value);
            node->token->value = NULL;
        }
        free(node->token);
        node->token = NULL;
    }
    if(node->name != NULL || strcmp(node->name, "") == 0) {
        //fprintf(stderr, "NODE WRONG NAME %s%d\n", node->name,node->type);
        free(node->name);
        node->name = NULL;
    }
    deleteSyntaxTree(node->left);
    destroyNodeList(node->statements);
    deleteSyntaxTree(node->right);
    free(node);
    node = NULL;
}
