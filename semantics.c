/**
 * @file semantics.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Implementation of semantics for IFJ20.
 */

#include "semantics.h"

/**
 * Parses string to TData type of data type
 * @param str String to parse
 * @return TData type
 */
TItem getDataTypeFromString(char* str) {
    if (strcmp(str, "int") == 0)
    {
        return TInt;
    }
    else if (strcmp(str, "float64") == 0)
    {
        return TDouble;
    }
    else
    {
        return TString;
    }
}

long parseStringToInt(char* str) {
    return strtol(str, NULL, 10);
}

char* parseIntToString(long num, tStringLinkedListItem* strList) {
    char* result = (char*) malloc(75 * sizeof(char));
    snprintf(result, 75, "%ld", num);
    addListItem(strList, result);   //remember this string
    return result;
}

double parseStringToFloat64(char* str) {
    return strtod(str, NULL);
}

char* parseFloat64ToString(double num, tStringLinkedListItem* strList) {
    char* result = (char*) malloc(75 * sizeof(char));
    snprintf(result, 75, "%lf", num);
    addListItem(strList, result);   //remember this string
    return result;
}

char* appendString(char* str1, char* str2, tStringLinkedListItem* strList) {
    unsigned size = strlen(str1) + strlen(str2) + 1;
    char* result = (char*) malloc(size * sizeof(char));
    snprintf(result, size, "%s%s", str1, str2);
    addListItem(strList, result);   //remember this string
    return result;
}

/**
 * Looks up for identifier in all reachable scopes
 * @param scope Scope where to start
 * @param name Name of identifier
 * @return tHashItem* if found, otherwise NULL
 */
tHashItem* getIdentifier(tScopeItem* scope, char* name){
    tHashItem *item = NULL;
    do {
        item = getHashItem(scope->table, name);
        if (item == NULL){
            scope = scope->next;
        }
    } while(item == NULL && scope != NULL);

    return item;
}

tExpReturnType identifierExp(SyntaxNode* root, tScope* scope) {
    tExpReturnType result;
    result.errCode = 0;
    result.value = NULL;
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->right->token->value;
    tHashItem *item = getIdentifier(currentScope, id);

    if (item == NULL) {
        fprintf(stderr, "Identifier %s is not declared!\n", id);
        result.errCode = 3;
    }

    result.type = item->type;
    result.value = item->value;

    return result;
}

/**
 * Calculate expression result type
 * @param root Pointer to the expressions's node
 * @param scope Pointer to current scope
 * @param strList Pointer to list of strings
 * @return tExpReturnType struct with type and data
 */
tExpReturnType evalExpression(SyntaxNode* root, tScope* scope, tStringLinkedListItem* strList) {
    enum typeOfNode type = root->type;
    tExpReturnType result;
    result.errCode = 0;
    result.value = "";

    if (type == Node_IdentifierExpression) {
        return identifierExp(root, scope);
    }
    else if (type == Node_NumberIntExpression) {
        result.type = TInt;
        result.value = root->right->token->value;
    }
    else if (type == Node_NumberDoubleExpression) {
        result.type = TDouble;
        result.value = root->right->token->value;
    }
    else if (type == Node_StringExpression) {
        result.type = TString;
        result.value = root->right->token->value;
    }
    else if (type == Node_UnaryExpression) {
        tExpReturnType rightSide = evalExpression(root->right, scope, strList);
        if (rightSide.type == TString) {
            result.errCode = 5;     //TODO is it 5?
            fprintf(stderr, "Unary operator does not work with string\n");
            return result;
        }
        result.type = rightSide.type;
        switch (root->token->type) {
            case tokenType_PLUS:
                break;
            case tokenType_MINUS:
                if (strlen(rightSide.value) > 0 && rightSide.value[0] == '-') {
                    rightSide.value += 1;
                    result.value = rightSide.value;
                }
                else {
                    char* res = appendString("-", rightSide.value, strList);
                    result.value = res;
                }
                return result;
            default:                //shouldn't happen at all
                result.errCode = 5;
                fprintf(stderr, "Unary operator is only + or -\n");
                return result;
        }
    }

    /*
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
    }*/
    return result;
}

/**
 * Declare variable. Returns error if already declared
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @return Return code (0 = OK)
 */
long declareExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->left->token->value;

    tHashItem *item = getIdentifier(currentScope, id);
    if (item != NULL) {
        fprintf(stderr, "Variable %s is already declared!\n", id);
        return 3;
    }

    tHashItem *parentFunc = getIdentifier(currentScope, parentFunction);
    if (parentFunc->func->params_count > 0) {
        for (int i = 0; i < parentFunc->func->params_count; i++) {
            if (strcmp(parentFunc->func->params[i], id) == 0)   //trying to declare identifier that has same name as parameter in function
            {
                fprintf(stderr, "Variable \"%s\" is declared as parameter of function \"%s\"!\n", id, parentFunction);
                return 3;
            }
        }
    }

    tExpReturnType result = evalExpression(root->right, scope, strList);
    if (result.errCode == 0) {
        addVarToHT(currentScope->table, id, result.type, result.value, true);
    }

    return result.errCode;
}

/**
 * Main switch to redirect code to other functions
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @return Return code (0 = OK)
 */
long statementMainSwitch(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    enum typeOfNode type = root->type;
    switch(type){
        case Node_DeclareExpression:                //DONE
            return declareExp(root, scope, parentFunction, strList);
        case Node_AssignmentExpression:
            break;
        case Node_ParenthezedExpression:
            break;
        case Node_IFExpression:
            break;
        case Node_ForExpression:
            break;
        case Node_FunctionCallExpression:
            break;
        case Node_ReturnExpression:
            break;

        //corner of hated expressions
        case Node_IdentifierExpression: break;
        case Node_BinaryExpression: break;

        case Node_BooleanExpression: break;

        case Node_NumberIntExpression:          //DONE
        case Node_NumberDoubleExpression:       //DONE
        case Node_StringExpression:             //DONE
        case Node_UnaryExpression:              //DONE
            return evalExpression(root, scope, strList).errCode;

        default:
            return 7;
    }

    return 0;
}


/**
 * WIP!!! Block expression node
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Function this code is under
 * @param strList Pointer to list of used strings
 * @return Return code (0 = OK)
 */
long blockExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    createScope(scope);

    SyntaxNodes *statement = root->statements != NULL ? root->statements->first : NULL;
    while (statement != NULL) {
        long returnCode = statementMainSwitch(statement->node, scope, parentFunction, strList);  //main logic

        if (returnCode != 0) {
            removeLastLocalScope(scope);
            return returnCode;
        }

        statement = statement->next;
    }

    removeLastLocalScope(scope);
    return 0;
}


/**
 * Function to check functions in code
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @return Return code (0 = OK)
 */
long runFunctionExp(SyntaxNode* root, tScope* scope, tStringLinkedListItem* strList){
    char* funcName = root->left->left->token->value;
    tHashTable* table = scope->topLocal->table;

    if (getHashItem(table, funcName) != NULL) {
        fprintf(stderr, "Function %s is already declared!\n", funcName);
        return 3;
    }

    addFuncToHT(table, funcName, true);
    SyntaxNodes *param = root->left->statements != NULL ? root->left->statements->first : NULL;
    while (param != NULL) {
        addParamToFunc(table, funcName, param->node->left->token->value, getDataTypeFromString(param->node->right->token->value));
        param = param->next;
    }
    SyntaxNodes *ret = root->statements != NULL ? root->statements->first : NULL;
    while (ret != NULL) {
        addReturnTypeToFunc(table, funcName, getDataTypeFromString(ret->node->token->value));
        ret = ret->next;
    }

    return blockExp(root->right, scope, funcName, strList);   //run things in the body of the function | also return it's errCode
}




/**
 * Adds all inbuilt functions to the global scope
 * @param scope Pointer to the global scope
 * @return Return code (0 = OK)
 */
long addInbuiltFunctions(tScope* scope) {
    tHashTable* table = scope->global->table;
    //func inputs() (string,int)
    addFuncToHT(table, "inputs", true);
    addReturnTypeToFunc(table, "inputs", TString);
    addReturnTypeToFunc(table, "inputs", TInt);
    //func inputi() (string,int)
    addFuncToHT(table, "inputi", true);
    addReturnTypeToFunc(table, "inputi", TInt);
    addReturnTypeToFunc(table, "inputi", TInt);
    //func inputf() (string,int)
    addFuncToHT(table, "inputf", true);
    addReturnTypeToFunc(table, "inputf", TDouble);
    addReturnTypeToFunc(table, "inputf", TInt);
    //func print ( term1 , term2 , …, term𝑛 )
    //addFuncToHT(table, "print", true);
    //addParamToFunc(table, "print",)

    //func int2float(i int) (float64)
    addFuncToHT(table, "int2float", true);
    addParamToFunc(table, "int2float", "i", TInt);
    addReturnTypeToFunc(table, "int2float", TDouble);

    //func float2int(f float64) (int)
    addFuncToHT(table, "float2int", true);
    addParamToFunc(table, "float2int", "f", TDouble);
    addReturnTypeToFunc(table, "float2int", TInt);

    //func len(s string) (int)
    addFuncToHT(table, "len", true);
    addParamToFunc(table, "len", "s", TString);
    addReturnTypeToFunc(table, "len", TInt);

    //func substr(s string, i int, n int) (string, int)
    addFuncToHT(table, "substr", true);
    addParamToFunc(table, "substr", "s", TString);
    addParamToFunc(table, "substr", "i", TInt);
    addParamToFunc(table, "substr", "n", TInt);
    addReturnTypeToFunc(table, "substr", TString);
    addReturnTypeToFunc(table, "substr", TInt);

    //func ord(s string, i int) (int, int)
    addFuncToHT(table, "ord", true);
    addParamToFunc(table, "ord", "s", TString);
    addParamToFunc(table, "ord", "i", TInt);
    addReturnTypeToFunc(table, "ord", TInt);
    addReturnTypeToFunc(table, "ord", TInt);

    //func chr(i int) (string, int)
    addFuncToHT(table, "chr", true);
    addParamToFunc(table, "chr", "i", TInt);
    addReturnTypeToFunc(table, "chr", TString);
    addReturnTypeToFunc(table, "chr", TInt);
    return 0;
}

/**
 * Main function of semantic analysis
 * @param root Start node of ASS
 * @return Return code (0 = OK)
 */
long runSemanticAnalyze(SyntaxNode* root){
    if(root == NULL){
        return 99;
    }

    tStringLinkedListItem strList;
    createList(&strList);

    tScope scope;
    initScope(&scope);
    createScope(&scope);

    addInbuiltFunctions(&scope);    //loads inbuild functions

    if (root->type == Node_Global){
        SyntaxNodes *statement = root->statements != NULL ? root->statements->first : NULL;
        while (statement != NULL){
            if (statement->node->type != Node_FunctionExpression){
                fprintf(stderr, "%s is outside of any function!\n", statement->node->name);
                return 3;
            }

            long returnCode = runFunctionExp(statement->node, &scope, &strList);  //evaluate expressions

            if (returnCode != 0) {
                removeLastLocalScope(&scope);
                return returnCode;
            }

            statement = statement->next;
        }
    }

    if (getHashItem(scope.topLocal->table, "main") != NULL) {               /** Checks if main function is declared as it should be */
        tHashItem* mainFunc = getHashItem(scope.topLocal->table, "main");
        if (mainFunc->declared == true && mainFunc->func != NULL && mainFunc->func->params_count == 0 && mainFunc->func->return_count == 0) {
            removeLastLocalScope(&scope);
            destroyList(&strList);
            return 0;
        }
        else {
            fprintf(stderr, "Main function should not have parameters or return value\n");
            removeLastLocalScope(&scope);

            destroyList(&strList);
            return 6;
        }
    }

    fprintf(stderr, "Main function not declared\n");
    removeLastLocalScope(&scope);
    destroyList(&strList);
    return 3;
}





/** OLD code of David's semantics */
/*
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
}*/

