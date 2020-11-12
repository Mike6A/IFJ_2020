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

