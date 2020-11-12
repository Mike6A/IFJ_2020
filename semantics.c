/**
 * @file semantics.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Implementation of semantics for IFJ20.
 */

#include "semantics.h"

/**
 * @return tHashItem* if found, otherwise NULL
 */
tHashItem* getIdentifier(tScopeItem* scope, char* name){
    tHashItem *item;
    do {
        item = getHashItem(scope->table, name);
        if (item == NULL){
            scope = scope->next;
        }
    } while(item == NULL && scope != NULL);

    return item;
}

void declareIdentifier(tScopeItem* scope, char* key, char* value) {
    fprintf(stderr, "INFO: Declaring identifier %s with value %s\n", key, value);
    addDataToHT(scope->table, key, value, true);
}



long checkNumberIntExp(SyntaxNode* root, tScope* scope){
    char* end;
    long result = strtol(root->right->token->value, &end, 10);
    printf("numIntExp >>> %ld\n", result);
    return result;
}

long checkIdentifierExp(SyntaxNode* root, tScope* scope){
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->right->token->value;
    tHashItem *item = getIdentifier(currentScope, id);

    if (item == NULL){
        fprintf(stderr, "ERROR: Identifier %s was not declared!\n", id);
        exit(3);
    }
    return 0;
}

long checkDeclareExp(SyntaxNode* root, tScope* scope){
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->left->token->value;
    tHashItem *item = getIdentifier(currentScope, id);

    if (item == NULL){
        declareIdentifier(currentScope, id, "1");
    }

    return 0;
}

long checkAssigntToExp(SyntaxNode* root, tScope* scope){
    tScopeItem *currentScope = scope->topLocal;

    char* id = "a";
    tHashItem* item = getIdentifier(currentScope, id);  //TODO loop through statements

    if (item == NULL){
        fprintf(stderr, "Identifier %s was not declared!\n", id);
        exit(3);
    }

    char* end;
    long result = strtol (item->value,&end,10);
    printf("\t%s\tRES >>> \t%ld\n", root->name,result);
    return result;
}



long runSemanticAnalyze(SyntaxNode* root, tScope* scope){
    if(root == NULL){
        return 0;
    }
    SyntaxNode* node = root;

    if (node-> type == Node_IdentifierExpression){
        checkIdentifierExp(node, scope);
    }
    else if (node->type == Node_AssignmentToExpression){
        checkAssigntToExp(node, scope);
    }
    else if (node->type == Node_DeclareExpression){
        checkDeclareExp(node, scope);
    }
    else if (node->type == Node_NumberIntExpression){
        checkNumberIntExp(node, scope);
    }

    runSemanticAnalyze(node->left, scope);
    SyntaxNodes *statement = node->statements != NULL ? node->statements->first : NULL;

    while (statement != NULL){
        runSemanticAnalyze(statement->node, scope);
        statement = statement->next;
    }
    runSemanticAnalyze(node->right, scope);

    return 0;
}


//TODO Free ctrl c/V
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

