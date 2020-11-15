/**
 * @file semantics.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Implementation of semantics for IFJ20.
 */

#include "semantics.h"

#define MAX_NUM2STRING_DIGITS 75
#define MAX_DOUBLE_ACCURACY 0.000000001

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
    char* result = (char*) malloc(MAX_NUM2STRING_DIGITS * sizeof(char));
    snprintf(result, MAX_NUM2STRING_DIGITS, "%ld", num);
    addListItem(strList, result);   //remember this string
    return result;
}

double parseStringToFloat64(char* str) {
    return strtod(str, NULL);
}

char* parseFloat64ToString(double num, tStringLinkedListItem* strList) {
    char* result = (char*) malloc(MAX_NUM2STRING_DIGITS * sizeof(char));
    snprintf(result, MAX_NUM2STRING_DIGITS, "%lf", num);
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

tExpReturnType identifierExp(SyntaxNode* root, tScope* scope, char* parentFunction) {
    tExpReturnType result;
    result.errCode = 0;
    result.value = "";
    result.constant = false;
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->right->token->value;
    tHashItem* item = getIdentifier(currentScope, id);

    if (item == NULL) {
        tHashItem* func = getIdentifier(currentScope, parentFunction);      //TODO test for recursion error finding bad variables
        if (func->func->params_count > 0) {
            for (int i = 0; i < func->func->params_count; i++) {
                if (strcmp(func->func->params[i], id) == 0)   //trying to find identifier in func params
                {
                    result.type = func->func->paramsTypes[i];
                    return result;
                }
            }
        }

        fprintf(stderr, "Identifier %s is not declared!\n", id);
        result.errCode = 3;
        return result;
    }

    result.type = item->type;
    result.constant = item->declared;
    if (result.constant) {
        result.value = item->value;
    }
    //TODO codegen: you'll need this variable, not sure how exactly
    return result;
}

/**
 * Calculate expression result type
 * @param root Pointer to the expressions's node
 * @param scope Pointer to current scope
 * @param strList Pointer to list of strings
 * @return tExpReturnType struct with type and data
 */
tExpReturnType evalExpression(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    enum typeOfNode type = root->type;
    tExpReturnType result;
    result.errCode = 0;
    result.value = "";
    result.constant = false;
    result.type = TString;

    if (type == Node_IdentifierExpression) {
        return identifierExp(root, scope, parentFunction);
    }
    else if (type == Node_NumberIntExpression) {
        result.type = TInt;
        result.value = root->right->token->value;
        result.constant = true;
    }
    else if (type == Node_NumberDoubleExpression) {
        result.type = TDouble;
        result.value = root->right->token->value;
        result.constant = true;
    }
    else if (type == Node_StringExpression) {
        result.type = TString;
        result.value = root->right->token->value;
        result.constant = true;
    }
    else if (type == Node_UnaryExpression) {
        tExpReturnType rightSide = evalExpression(root->right, scope, parentFunction, strList);
        if (rightSide.type == TString) {
            result.errCode = 5;     //TODO is it 5?
            fprintf(stderr, "Unary operator does not work with string\n");
            return result;
        }
        //TODO codegen: from this point, if we don't use optimization
        if (rightSide.constant == false) {
            return rightSide;
        }

        result.type = rightSide.type;
        result.constant = true;
        switch (root->token->type) {
            case tokenType_PLUS:
                result.value = rightSide.value;
                return result;
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
    else if (type == Node_BinaryExpression) {
        tExpReturnType rightSide = evalExpression(root->right, scope, parentFunction, strList);
        if (rightSide.errCode != 0) {   //check return code for errors
            result.errCode = rightSide.errCode;
            return result;
        }
        tExpReturnType leftSide = evalExpression(root->left, scope, parentFunction, strList);
        if (leftSide.errCode != 0) {    //check return code for errors
            result.errCode = leftSide.errCode;
            return result;
        }

        if (leftSide.type != rightSide.type) {      //types must be same
            result.errCode = 5;
            fprintf(stderr, "Types in binary operation must be the same!\n");
            return result;
        }

        TokenType tokenType = root->token->type;
        if (rightSide.type == TString) {                //string
            result.type = TString;
            if (tokenType == tokenType_PLUS) {
                //TODO codegen: string concatenate, values -> leftSide.constant => leftSide.value | rightSide.constant => rightSide.value
                if (rightSide.constant == true && leftSide.constant == true) {
                    result.value = appendString(leftSide.value, rightSide.value, strList);
                    result.constant = true;
                }
                return result;
            }
            else {  //minus, div, mul
                fprintf(stderr, "Only plus (concatenation) is valid operation with strings!\n");
                result.errCode = 5;
            }
        }
        else if (rightSide.type == TInt) {                  //int
            result.type = TInt;
            if (rightSide.constant == true && tokenType == tokenType_DIV) {
                if (parseStringToInt(rightSide.value) == 0){
                    fprintf(stderr, "Division by zero is not allowed!\n");
                    result.errCode = 9;
                    return result;
                }
            }
            //TODO codegen: int operations, values -> leftSide.constant => leftSide.value | tokenType | rightSide.constant => rightSide.value

            if (rightSide.constant == false || leftSide.constant == false) {
                if (tokenType == tokenType_MUL) {
                    if (rightSide.constant != leftSide.constant){
                        long tmp = 1;
                        if (rightSide.constant == true) {
                            tmp = parseStringToInt(rightSide.value);
                        }
                        else if (leftSide.constant == true) {
                            tmp = parseStringToInt(leftSide.value);
                        }
                        if (tmp == 0) {
                            result.value = "0";
                            result.constant = true;
                        }
                    }
                }

                return result;
            }

            long right = parseStringToInt(rightSide.value);
            long left = parseStringToInt(leftSide.value);
            result.constant = true;
            if (tokenType == tokenType_PLUS) {
                result.value = parseIntToString(left + right, strList);
            }
            else if (tokenType == tokenType_MINUS) {
                result.value = parseIntToString(left - right, strList);
            }
            else if (tokenType == tokenType_MUL) {
                result.value = parseIntToString(left * right, strList);
            }
            else if (tokenType == tokenType_DIV) {
                result.value = parseIntToString(left / right, strList);
            }
        }
        else if (rightSide.type == TDouble) {                   //float64
            result.type = TDouble;
            if (rightSide.constant == true && tokenType == tokenType_DIV) {
                double right = parseStringToFloat64(rightSide.value);
                if (right > -MAX_DOUBLE_ACCURACY && right < MAX_DOUBLE_ACCURACY)        //TODO accuracy
                {
                    fprintf(stderr, "Division by zero is not allowed!\n");
                    result.errCode = 9;
                    return result;
                }
            }
            //TODO codegen: double operations, values -> leftSide.constant => leftSide.value | tokenType | rightSide.constant => rightSide.value
            if (rightSide.constant == false || leftSide.constant == false) {
                if (tokenType == tokenType_MUL) {
                    if (rightSide.constant != leftSide.constant){
                        double tmp = 1;
                        if (rightSide.constant == true) {
                            tmp = parseStringToFloat64(rightSide.value);
                        }
                        else if (leftSide.constant == true) {
                            tmp = parseStringToFloat64(leftSide.value);
                        }
                        if (tmp > MAX_DOUBLE_ACCURACY && tmp < MAX_DOUBLE_ACCURACY) {
                            result.value = "0.0";
                            result.constant = true;
                        }
                    }
                }
                return result;
            }

            double left = parseStringToFloat64(leftSide.value);
            double right = parseStringToFloat64(rightSide.value);
            result.constant = true;
            if (tokenType == tokenType_PLUS) {
                result.value = parseFloat64ToString(left + right, strList);
            }
            else if (tokenType == tokenType_MINUS) {
                result.value = parseFloat64ToString(left - right, strList);

            }
            else if (tokenType == tokenType_MUL) {
                result.value = parseFloat64ToString(left * right, strList);

            }
            else if (tokenType == tokenType_DIV) {
                result.value = parseFloat64ToString(left / right, strList);
            }
        }
        else {      //else if (rightSide.type == TBool) -> only bool should fall to this else
            fprintf(stderr, "Using booleans in binary operations is not allowed!\n");
            result.errCode = 5;
        }

    }
    else if (type == Node_ParenthezedExpression) {
        SyntaxNodes *statement = root->statements != NULL ? root->statements->first : NULL;
        if (statement != NULL) {
            result = evalExpression(statement->node, scope, parentFunction, strList);
        }
    }
    else if (type == Node_BooleanExpression) {
        result.type = TBool;
        tExpReturnType rightSide = evalExpression(root->right, scope, parentFunction, strList);
        if (rightSide.errCode != 0) {   //check return code for errors
            result.errCode = rightSide.errCode;
            return result;
        }
        tExpReturnType leftSide = evalExpression(root->left, scope, parentFunction, strList);
        if (leftSide.errCode != 0) {    //check return code for errors
            result.errCode = leftSide.errCode;
            return result;
        }

        if (leftSide.type != rightSide.type) {      //types must be same
            result.errCode = 5;
            fprintf(stderr, "Types in boolean operation must be the same!\n");
            return result;
        }

        TokenType tokenType = root->token->type;
        if (tokenType != tokenType_GREATER && tokenType != tokenType_LESS && tokenType != tokenType_GE && tokenType != tokenType_LE && tokenType != tokenType_EQ  && tokenType != tokenType_NEQ) {
            result.value = "0";
            result.errCode = 99;
            fprintf(stderr, "Unexpected operator inside boolean expression!\n");
            return result;      //shouldn't happen
        }

        //TODO codegen: boolean compare, values -> leftSide.constant => leftSide.value | tokenType | rightSide.constant => rightSide.value
        if (rightSide.type == TInt) {
            if (leftSide.constant == false || rightSide.constant == false) {
                return result;
            }
            long right = parseStringToInt(rightSide.value);
            long left = parseStringToInt(leftSide.value);
            result.constant = true;

            long isTrue = 0;
            switch (tokenType) {
                case tokenType_GREATER:
                    isTrue = left > right;
                    break;
                case tokenType_LESS:
                    isTrue = left < right;
                    break;
                case tokenType_GE:
                    isTrue = left >= right;
                    break;
                case tokenType_LE:
                    isTrue = left <= right;
                    break;
                case tokenType_EQ:
                    isTrue = left == right;
                    break;
                //case tokenType_NEQ:
                default:
                    isTrue = left != right;
                    break;
            }
            result.value = parseIntToString(isTrue, strList);
        }
        else if (rightSide.type == TDouble) {
            if (leftSide.constant == false || rightSide.constant == false) {
                return result;
            }
            double right = parseStringToFloat64(rightSide.value);
            double left = parseStringToFloat64(leftSide.value);
            result.constant = true;

            long isTrue = 0;
            switch (tokenType) {
                case tokenType_GREATER:
                    isTrue = left > right;
                    break;
                case tokenType_LESS:
                    isTrue = left < right;
                    break;
                case tokenType_GE:
                    isTrue = left >= right;
                    break;
                case tokenType_LE:
                    isTrue = left <= right;
                    break;
                case tokenType_EQ:
                    isTrue = left == right;
                    break;
                //case tokenType_NEQ:
                default:
                    isTrue = left != right;
                    break;
            }
            result.value = parseFloat64ToString(isTrue, strList);
        }
        else if (rightSide.type == TString) {
            if (leftSide.constant == false || rightSide.constant == false) {
                return result;
            }
            int cmp = strcmp(leftSide.value, rightSide.value);  //+ -> left is greater | 0 -> same | - -> right is greater

            long isTrue = 0;
            switch (tokenType) {
                case tokenType_GREATER:
                    isTrue = (cmp > 0);
                    break;
                case tokenType_LESS:
                    isTrue =  (cmp < 0);
                    break;
                case tokenType_GE:
                    isTrue =  (cmp >= 0);
                    break;
                case tokenType_LE:
                    isTrue = (cmp <= 0);
                    break;
                case tokenType_EQ:
                    isTrue = (cmp == 0);
                    break;
                //case tokenType_NEQ:
                default:
                    isTrue = (cmp != 0);
                    break;
            }
            result.value = parseIntToString(isTrue, strList);
        }
        else {      //else if (rightSide.type == TBool) -> only bool should fall to this else | BOOLTHEN -> add condition for == or !=
            fprintf(stderr, "Using booleans as operands in boolean expression is not allowed!\n");
            result.errCode = 5;
        }

    }

    return result;
}

/**
 * Checks return semantics, if the return types are as it should be
 * @param root Pointer to return node
 * @param scope Pointer to current scope
 * @param parentFunction Name of function this return belongs to
 * @param strList Pointer to list of string to delete
 * @return Return code (0 = OK)
 */
long returnExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    int returnCode = 0;
    tHashItem* func = getIdentifier(scope->topLocal, parentFunction);
    if (func != NULL) {     //function is declared (if not, there's something wrong)
        int retCount = 0;
        SyntaxNodes *retVal = root->statements != NULL ? root->statements->first : NULL;
        while (retVal != NULL) {
            tExpReturnType retItem = evalExpression(retVal->node, scope, parentFunction, strList);
            if (retCount < func->func->return_count) {      //check the number of parameters
                if (retItem.type != func->func->return_vals[retCount]) {
                    fprintf(stderr, "%d. return from function \"%s\" is an incompatible type!\n", retCount+1, parentFunction);
                    returnCode = 6;
                }
            }
            else {
                fprintf(stderr, "Return from function \"%s\" has more parameters than defined!\n", parentFunction);
                returnCode = 6;
            }

            if (returnCode != 0) {  //exit
                return returnCode;
            }
            retCount++;
            retVal = retVal->next;
        }
    }
    else {
        fprintf(stderr, "Return of function \"%s\" is defined, but function not...something went wrong...\n", parentFunction);
        returnCode = 6;
        return returnCode;
    }

    //TODO codegen: return variables + destroy scope
    return returnCode;
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

    tExpReturnType result = evalExpression(root->right, scope, parentFunction, strList);
    if (result.errCode == 0) {
        addVarToHT(currentScope->table, id, result.type, result.value, result.constant);
        //TODO codegen: declare variable in current scope | name: id, value: result.value, constant: result.constant
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
    switch(type) {
        case Node_DeclareExpression:                //DONE
            return declareExp(root, scope, parentFunction, strList);
        case Node_AssignmentExpression:
            break;
        case Node_IFExpression:
            break;
        case Node_ForExpression:
            break;
        case Node_FunctionCallExpression:
            break;
        case Node_ReturnExpression:             //DONE
            return returnExp(root, scope, parentFunction, strList);

        case Node_IdentifierExpression:         //DONE
            return identifierExp(root, scope, parentFunction).errCode;

        case Node_BooleanExpression:            //DONE
        case Node_ParenthezedExpression:        //DONE
        case Node_BinaryExpression:             //DONE
        case Node_NumberIntExpression:          //DONE
        case Node_NumberDoubleExpression:       //DONE
        case Node_StringExpression:             //DONE
        case Node_UnaryExpression:              //DONE
            return evalExpression(root, scope, parentFunction, strList).errCode;

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

    //TODO codegen: destroy scope, shouldn't be used when scope is already destroyed with return!
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

    //TODO codegen: function head + scope + parameters
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

    tStringLinkedListItem* strList = malloc(sizeof(tStringLinkedListItem));
    createList(strList);

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

            long returnCode = runFunctionExp(statement->node, &scope, strList);  //evaluate expressions

            if (returnCode != 0) {
                removeLastLocalScope(&scope);
                destroyList(strList);
                return returnCode;
            }

            statement = statement->next;
        }
    }

    if (getHashItem(scope.topLocal->table, "main") != NULL) {               /** Checks if main function is declared as it should be */
        tHashItem* mainFunc = getHashItem(scope.topLocal->table, "main");
        if (mainFunc->declared == true && mainFunc->func != NULL && mainFunc->func->params_count == 0 && mainFunc->func->return_count == 0) {
            removeLastLocalScope(&scope);
            destroyList(strList);
            //TODO codegen: now you can throw the program to the interpret
            return 0;
        }
        else {
            fprintf(stderr, "Main function should not have parameters or return value\n");
            removeLastLocalScope(&scope);

            destroyList(strList);
            return 6;
        }
    }

    fprintf(stderr, "Main function not declared\n");
    removeLastLocalScope(&scope);
    destroyList(strList);
    return 3;
}





/** OLD code of David's semantics */
/*
long eval(tTokenizer* tokenizer, SyntaxNode * root, tScope* scope){

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

    return -1;
}*/

