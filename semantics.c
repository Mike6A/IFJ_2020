/**
 * @file semantics.c
 * @author Radim Buráň (xburan03@stud.fit.vutbr.cz)
 * @brief Implementation of semantics for IFJ20.
 */

#include "semantics.h"
#include "gen_code.h"

#define MAX_NUM2STRING_DIGITS 75
#define MAX_DOUBLE_ACCURACY 0.000000001

long blockExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList);
bool foundReturn;        /** Variable to check if all routes of code have return statement (if/else) */
bool disableAssignment;  /** Disables assignments where isn't clear at compile time, what will happen (if/else and for) */

/**
 * Parse string to TItem type of data type
 * @param str String to parse
 * @return TItem struct type
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

/**
 * Function to parse String to Int
 * @param str String to parse
 * @return Int value of parsed string
 */
long parseStringToInt(char* str) {
    return strtol(str, NULL, 10);
}

/**
 * Function to parse Int to string
 * @param num Int value
 * @param strList Pointer to temp string list
 * @return String from number
 */
char* parseIntToString(long num, tStringLinkedListItem* strList) {
    char* result = (char*) malloc(MAX_NUM2STRING_DIGITS * sizeof(char));
    snprintf(result, MAX_NUM2STRING_DIGITS, "%ld", num);
    addListItem(strList, result);   //remember this string
    return result;
}

/**
 * Function to parse String to Float64
 * @param str String to parse
 * @return Float64 value of parsed string
 */
double parseStringToFloat64(char* str) {
    return strtod(str, NULL);
}

/**
 * Function to parse Float64 to string
 * @param num Float64 value
 * @param strList Pointer to temp string list
 * @return String from number
 */
char* parseFloat64ToString(double num, tStringLinkedListItem* strList) {
    char* result = (char*) malloc(MAX_NUM2STRING_DIGITS * sizeof(char));
    snprintf(result, MAX_NUM2STRING_DIGITS, "%lf", num);
    addListItem(strList, result);   //remember this string
    return result;
}

/**
 * Function to append strings
 * @param str1 First string
 * @param str2 Second string
 * @param strList Pointer to temp string list
 * @return Merged string
 */
char* appendString(char* str1, char* str2, tStringLinkedListItem* strList) {
    unsigned size = strlen(str1) + strlen(str2) + 1;
    char* result = (char*) malloc(size * sizeof(char));
    snprintf(result, size, "%s%s", str1, str2);
    addListItem(strList, result);   //remember this string to free it later
    return result;
}

/**
 * Looks up for identifier in all reachable scopes
 * @param scope Scope where to start
 * @param name Name of identifier
 * @param scopeLevel Changes value of int at this pointer to the level of scope
 * @return tHashItem* if found, otherwise NULL
 */
tHashItem* getIdentifier(tScopeItem* scope, char* name, int* scopeLevel) {
    tHashItem *item = NULL;
    do {
        item = getHashItem(scope->table, name);
        if (item == NULL){
            scope = scope->next;
            if (scopeLevel != NULL) {
                *scopeLevel = scope->scopeLevel;
            }
        }
    } while(item == NULL && scope != NULL);

    if (item == NULL && scopeLevel != NULL) {
        *scopeLevel = -1;
    }

    return item;
}

/**
 * Function to handle identifiers
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @return Return code (0 = OK)
 */
tExpReturnType identifierExp(SyntaxNode* root, tScope* scope, char* parentFunction) {
    tExpReturnType result;
    result.errCode = 0;
    result.value = "";
    result.constant = false;
    tScopeItem *currentScope = scope->topLocal;

    char* id;
    if (root->right != NULL)
        id = root->right->token->value;
    else
        id = root->left->token->value;

    if (strcmp(id, "_") == 0) {
        result.type = TEverything;
        return result;
    }

    tHashItem* item = getIdentifier(currentScope, id, NULL);
    if (item != NULL && item->func != NULL) {
        item = NULL;
    }

    if (item == NULL) {
        tHashItem* func = getIdentifier(scope->topLocal, parentFunction, NULL);
        if (func->func->params_count > 0) {
            for (int i = 0; i < func->func->params_count; i++) {
                if (strcmp(func->func->params[i], id) == 0)   //trying to find identifier in func params
                {
                    result.type = func->func->paramsTypes[i];
                    return result;
                }
            }
        }

        //fprintf(stderr, "Identifier %s is not declared!\n", id);
        result.errCode = 3;
        return result;
    }

    result.type = item->type;
    result.constant = item->declared;
    if (result.constant) {
        result.value = item->value;
    }
    return result;
}

/**
 * Calculate expression result type
 * @param root Pointer to the expressions's node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
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
            result.errCode = 5;
            //fprintf(stderr, "Unary operator does not work with string\n");
            return result;
        }
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
                //fprintf(stderr, "Unary operator is only + or -\n");
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

        if (leftSide.type == TEverything || rightSide.type == TEverything) {
            result.errCode = 3;
            //fprintf(stderr, "Can't use underscore in binary operations!\n");
            return result;
        }

        if (leftSide.type != rightSide.type) {      //types must be same
            result.errCode = 5;
            //fprintf(stderr, "Types in binary operation must be the same!\n");
            return result;
        }

        TokenType tokenType = root->token->type;
        if (rightSide.type == TString) {                //string
            result.type = TString;
            if (tokenType == tokenType_PLUS) {
                if (rightSide.constant == true && leftSide.constant == true) {
                    result.value = appendString(leftSide.value, rightSide.value, strList);
                    result.constant = true;
                }
                return result;
            }
            else {  //minus, div, mul
                //fprintf(stderr, "Only plus (concatenation) is valid operation with strings!\n");
                result.errCode = 5;
            }
        }
        else if (rightSide.type == TInt) {                  //int
            result.type = TInt;
            if (rightSide.constant == true && tokenType == tokenType_DIV) {
                if (parseStringToInt(rightSide.value) == 0){
                    //fprintf(stderr, "Division by zero is not allowed!\n");
                    result.errCode = 9;
                    return result;
                }
            }

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
               // printf("%d %d\n",left, right);
            }
            else if (tokenType == tokenType_DIV) {
                result.value = parseIntToString(left / right, strList);
            }
        }
        else if (rightSide.type == TDouble) {                   //float64
            result.type = TDouble;
            if (rightSide.constant == true && tokenType == tokenType_DIV) {
                double right = parseStringToFloat64(rightSide.value);
                if (right > -MAX_DOUBLE_ACCURACY && right < MAX_DOUBLE_ACCURACY)
                {
                    //fprintf(stderr, "Division by zero is not allowed!\n");
                    result.errCode = 9;
                    return result;
                }
            }
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
            //fprintf(stderr, "Using booleans in binary operations is not allowed!\n");
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
            //fprintf(stderr, "Types in boolean operation must be the same!\n");
            return result;
        }

        TokenType tokenType = root->token->type;
        if (tokenType != tokenType_GREATER && tokenType != tokenType_LESS && tokenType != tokenType_GE && tokenType != tokenType_LE && tokenType != tokenType_EQ  && tokenType != tokenType_NEQ) {
            result.value = "0";
            result.errCode = 99;
            //fprintf(stderr, "Unexpected operator inside boolean expression!\n");
            return result;      //shouldn't happen
        }

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
        else {      //else if (rightSide.type == TBool) -> only bool should fall to this else
            //fprintf(stderr, "Using booleans as operands in boolean expression is not allowed!\n");
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
    tHashItem* func = getIdentifier(scope->topLocal, parentFunction, NULL);
    if (func != NULL) {     //function is declared (if not, there's something wrong)
        int retCount = 0;
        SyntaxNodes *retVal = root->statements != NULL ? root->statements->first : NULL;
        while (retVal != NULL) {
            tExpReturnType retItem = evalExpression(retVal->node, scope, parentFunction, strList);
            if (retCount < func->func->return_count) {      //check the number of parameters
                if (retItem.type != func->func->return_vals[retCount]) {
                    //fprintf(stderr, "%d. return from function \"%s\" is an incompatible type!\n", retCount+1, parentFunction);
                    returnCode = 6;
                }
            }
            else {
                //fprintf(stderr, "Return from function \"%s\" has more parameters than defined!\n", parentFunction);
                returnCode = 6;
            }

            if (returnCode != 0) {  //exit
                return returnCode;
            }
            retCount++;
            retVal = retVal->next;
        }

        if (retCount != func->func->return_count) {
            //fprintf(stderr, "Return from function \"%s\" has less parameters than defined!\n", parentFunction);
            returnCode = 6;
        }
    }
    else {
        //fprintf(stderr, "Return of function \"%s\" is defined, but function not...something went wrong...\n", parentFunction);
        returnCode = 6;
        return returnCode;
    }

    foundReturn = true;
    if (returnCode == 0 && root->statements!=NULL && root->statements->first != NULL)
        func_ret_get_value(parentFunction, func->func, root->statements->first);
    return returnCode;
}

/**
 * Declare variable, check if the variable isn't already declared
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
long declareExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->left->token->value;

    tHashItem *item = getHashItem(currentScope->table, id);
    if (item != NULL) {
        //fprintf(stderr, "Variable %s is already declared!\n", id);
        return 3;
    }

    tHashItem *parentFunc = getIdentifier(scope->topLocal, parentFunction, NULL);
    if (parentFunc->func->params_count > 0) {
        for (int i = 0; i < parentFunc->func->params_count; i++) {
            if (strcmp(parentFunc->func->params[i], id) == 0)   //trying to declare identifier that has same name as parameter in function
            {
                //fprintf(stderr, "Variable \"%s\" is declared as parameter of function \"%s\"!\n", id, parentFunction);
                return 3;
            }
        }
    }

    tExpReturnType result = evalExpression(root->right, scope, parentFunction, strList);
    if (result.errCode == 0) {
        addVarToHT(currentScope->table, id, result.type, result.value, result.constant);
        vars_default_declar_init(root->right, getIdentifier(currentScope, id, NULL));
    }

    return result.errCode;
}

/**
 * Function to assignment to an identifier
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @return tExpReturnType struct with type and data
 */
tExpReturnType assignmentExpSingleIdentifier(SyntaxNode* root, tScope* scope, char* parentFunction) {
    tExpReturnType result;
    result.errCode = 0;
    result.value = "";
    result.constant = false;
    tScopeItem *currentScope = scope->topLocal;
    char* id = root->left->token->value;
    tHashItem* item = getIdentifier(currentScope, id, NULL);

    if (item == NULL) {
        if (strcmp(id, "_") == 0) {
            result.type = TEverything;
            return result;
        }

        tHashItem* func = getIdentifier(scope->topLocal, parentFunction, NULL);
        if (func->func->params_count > 0) {
            for (int i = 0; i < func->func->params_count; i++) {
                if (strcmp(func->func->params[i], id) == 0)   //trying to find identifier in func params
                {
                    result.type = func->func->paramsTypes[i];
                    return result;
                }
            }
        }

        //fprintf(stderr, "Identifier %s is not declared!\n", id);
        result.errCode = 3;
        return result;
    }

    result.type = item->type;

    if (disableAssignment == false) {
        result.constant = item->declared;
        if (result.constant) {
            result.value = item->value;
            item->declared = true;
        }
    }
    else {
        item->declared = false;
    }
    return result;
}

/**
 * Function to handle single assignment between two nodes
 * @param dest Pointer to the destination node
 * @param value Pointer to the value node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
long assignmentExpSingle(SyntaxNode* dest, SyntaxNode* value, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    tExpReturnType leftSide = assignmentExpSingleIdentifier(dest, scope, parentFunction);
    if (leftSide.errCode != 0) {
        return leftSide.errCode;
    }
    tExpReturnType rightSide = evalExpression(value, scope, parentFunction, strList);   //gut
    if (rightSide.errCode != 0) {
        return rightSide.errCode;
    }

    if (leftSide.type != rightSide.type && leftSide.type != TEverything) {
        //fprintf(stderr, "Assignment of incompatible types!\n");
        return 5;
    }

    tHashItem* item = getIdentifier(scope->topLocal, dest->left->token->value, NULL); //if item is null -> function param
    if (item != NULL) {
        item->value = realloc(item->value, sizeof(char) * (strlen(rightSide.value) + 1));     //need to copy string or it will destroy everything (bad pointer)
        if (item->value == NULL)
            return 99;
        strcpy(item->value, rightSide.value);
    }
    if(item != NULL)
        vars_set_new_value(value, item);
    else{
        if(leftSide.type != TEverything){
            tHashItem itemFuncParam = {.type = leftSide.type, .id = dest->left->token->value};
            vars_set_new_value(value, &itemFuncParam);
        }
    }
    return 0;
}

/**
 * Main function to handle different assignments
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
long assignmentExp(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    SyntaxNodes *destination = root->left->statements != NULL ? root->left->statements->first : NULL;
    SyntaxNodes *value = root->right->statements != NULL ? root->right->statements->first : NULL;
    int returnCode = 0;

    if (value->node->type == Node_FunctionCallExpression) { //values from return from function
        tHashItem* tItem = getIdentifier(scope->topLocal, value->node->token->value, NULL);
        if (tItem != NULL) {
            tFuncItem* func = tItem->func;
            //function is already defined
            if (func != NULL) {
                if (strcmp("print", value->node->token->value) == 0) {   //print can have more parameters
                    //fprintf(stderr, "Function \"print\" has zero return parameters!");
                    return 6;
                }
                int index = 0;      //check function parameters
                SyntaxNodes *param = value->node->statements != NULL ? value->node->statements->first : NULL;
                while (param != NULL) {
                    tExpReturnType paramItem = evalExpression(param->node, scope, parentFunction, strList);
                    if (paramItem.errCode != 0) {
                        return paramItem.errCode;
                    }

                    if (paramItem.type == TEverything) {
                        //fprintf(stderr, "You can't use underscore in function \"%s\" parameter!\n", value->node->token->value);
                        return 3;
                    }

                    if (index >= func->params_count || paramItem.type != func->paramsTypes[index]) {
                        //fprintf(stderr, "Function \"%s\" has been called with different parameters!", value->node->token->value);
                        return 6;
                    }
                    param = param->next;
                    index++;
                }
                if (index != func->params_count) {
                    //fprintf(stderr, "Function \"%s\" has been called with different parameters!", value->node->token->value);
                    return 6;
                }

                if (destination->node == NULL) {    //nobody wants return values from this function. Poor function
                    return 0;
                }
                func_args_TF_declar(value->node->token->value, tItem, value->node->statements);
                general_func_call(value->node->token->value);
                SyntaxNodes *tmpDest = destination;
                for (int i = 0; i < func->return_count; i++) {
                    if (destination == NULL) {
                        //fprintf(stderr, "Not using all return values from function \"%s\"!\n", value->node->token->value);
                        return 6;
                    }
                    disableAssignment = true;
                    tExpReturnType leftSide = assignmentExpSingleIdentifier(destination->node, scope, parentFunction);
                    disableAssignment = false;
                    if (leftSide.errCode != 0) {
                        return leftSide.errCode;
                    }
                    if (leftSide.type != func->return_vals[i] && leftSide.type != TEverything) {
                        //fprintf(stderr, "Incompatible types in assignment of function return values!\n");
                        return 6;
                    }
                    destination = destination->next;
                }

                if (destination != NULL) {
                    //fprintf(stderr, "Function \"%s\" does only return %d values!\n", value->node->token->value, func->return_count);
                    return 6;
                }
                func_ret_to_LF(value->node->token->value, func, tmpDest);

            }
            else {  //shouldn't happen, because only functions are defined in global scope
                //fprintf(stderr, "\"%s\" is already defined as a variable, function needs another name!", value->node->token->value);
                return 3;
            }
        }   //function is not defined
        else {
            addFuncToHT(scope->global->table, value->node->token->value, false);

            SyntaxNodes *param = value->node->statements != NULL ? value->node->statements->first : NULL;
            while (param != NULL) {
                tExpReturnType paramItem = evalExpression(param->node, scope, parentFunction, strList);
                if (paramItem.errCode != 0) {
                    return paramItem.errCode;
                }
                addParamToFunc(scope->global->table, value->node->token->value, "", paramItem.type);
                param = param->next;
            }
            tItem = getHashItem(scope->global->table, value->node->token->value);
            func_args_TF_declar(value->node->token->value, tItem, value->node->statements);
            general_func_call(value->node->token->value);
            SyntaxNodes *tmpDest = destination;
            while (destination != NULL) {
                disableAssignment = true;
                tExpReturnType paramItem = assignmentExpSingleIdentifier(destination->node, scope, parentFunction);
                disableAssignment = false;
                if (paramItem.errCode != 0) {
                    return paramItem.errCode;
                }
                addReturnTypeToFunc(scope->global->table, value->node->token->value, paramItem.type);
                destination = destination->next;
            }
            func_ret_to_LF(value->node->token->value, tItem->func, tmpDest);
        }
    }
    else {  //normal values
        while (destination != NULL && value != NULL) {
            returnCode = assignmentExpSingle(destination->node, value->node, scope, parentFunction, strList);
            if (returnCode != 0) {
                return returnCode;
            }
            destination = destination->next;
            value = value->next;
        }
        if (value != NULL || destination != NULL) {     //number of expressions isn't equal
            return 6;
        }
    }

    return 0;
}

/**
 * Function to handle function call (without assignment)
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
int callFunction(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    int result = 0;
    tHashItem* tItem = getIdentifier(scope->topLocal, root->token->value, NULL);
    if (tItem != NULL) {
        tFuncItem* func = tItem->func;
        //function is already defined
        if (func != NULL) {
            if (strcmp("print", root->token->value) == 0) {   //print can have more parameters
                bif_print(root->statements);
                return 0;
            }
            int index = 0;      //check function parameters
            SyntaxNodes *param = root->statements != NULL ? root->statements->first : NULL;
            while (param != NULL) {
                tExpReturnType paramItem = evalExpression(param->node, scope, parentFunction, strList);
                if (paramItem.errCode != 0) {
                    return paramItem.errCode;
                }

                if (paramItem.type == TEverything) {
                    //fprintf(stderr, "You can't use underscore in function \"%s\" parameter!\n", root->token->value);
                    return 3;
                }

                if (index >= func->params_count || paramItem.type != func->paramsTypes[index]) {
                    //fprintf(stderr, "Function \"%s\" has been called with different parameters!", root->token->value);
                    return 6;
                }
                param = param->next;
                index++;
            }
            if (index != func->params_count) {
                //fprintf(stderr, "Function \"%s\" has been called with different parameters!", root->token->value);
                return 6;
            }

        }
        else {  //shouldn't happen, because only functions are defined in global scope
            //fprintf(stderr, "\"%s\" is already defined as a variable, function needs another name!", root->token->value);
            return 3;
        }

    }   //function is not defined
    else {
        addFuncToHT(scope->global->table, root->token->value, false);

        SyntaxNodes *param = root->statements != NULL ? root->statements->first : NULL;
        while (param != NULL) {
            tExpReturnType paramItem = evalExpression(param->node, scope, parentFunction, strList);
            if (paramItem.type == TEverything) {
                //fprintf(stderr, "You can't use underscore in function \"%s\" parameter!\n", root->token->value);
                return 3;
            }
            if (paramItem.errCode != 0) {
                return paramItem.errCode;
            }
            addParamToFunc(scope->global->table, root->token->value, "", paramItem.type);
            param = param->next;
        }

    }
    tHashItem* tItemFunc = getIdentifier(scope->topLocal, root->token->value, NULL);
    func_args_TF_declar(root->token->value, tItemFunc, root->statements != NULL ? root->statements->first: NULL);
    general_func_call(root->token->value);
    return result;
}

/**
 * Function to handle If expression
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
int ifExpression(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    tExpReturnType condition = evalExpression(root->left, scope, parentFunction, strList);
    if (condition.errCode != 0) {
        return condition.errCode;
    }

    if (condition.type != TBool) {
        //fprintf(stderr, "Bad expression in if condition!\n");
        return 5;
    }
    //GENERATE
    if_cond(root->left, parentFunction);
    if_prefix(parentFunction);
    disableAssignment = true;
    int result = blockExp(root->statements->first->node, scope, parentFunction, strList);    //true
    if (result != 0) {
        return result;
    }
    if_suffix(parentFunction);
    else_prefix(parentFunction);
    result = blockExp(root->right->right, scope, parentFunction, strList);    //else
    if (result != 0) {
        return result;
    }
    else_suffix(parentFunction);
    label_if_else_end(parentFunction);
    disableAssignment = false;

    return 0;
}

/**
 * Function to handle For expression
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
int forExpression(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    int result = 0;
    createScope(scope);
    for_header(); // GENCODE

    //------- declaration part -------- -> optional
    if (root->left->left != NULL) {
        if (root->left->left->type == Node_DeclareExpression) {
            result = declareExp(root->left->left, scope, parentFunction, strList);
        }
    }


    if (result != 0) {
        removeLastLocalScope(scope);
        return result;
    }

    //------- condition part --------- -> obligatory
    tExpReturnType condition = evalExpression(root->left->right, scope, parentFunction, strList);
    if (condition.errCode != 0) {
       result = condition.errCode;
    }

    if (result != 0) {
        removeLastLocalScope(scope);
        return result;
    }

    if (condition.type != TBool) {
        //fprintf(stderr, "Bad expression in for condition!\n");
        result = 5;
    }
    if (result != 0) {
        removeLastLocalScope(scope);
        return result;
    }

    for_cond_to_loop(root->left->right, parentFunction);
    for_afterDeclaration(parentFunction);// GENCODE
    //------- block part ------

    result = blockExp(root->right, scope, parentFunction, strList);    //block with things to loop in for
    if (result != 0) {
        removeLastLocalScope(scope);
        return result;
    }
    for_start_Assign();
    //------- assignment part ------ -> optional
    if (root->left->statements != NULL) {
        if (root->left->statements->first->node->type != Node_AssignmentExpression) {
            //fprintf(stderr, "Bad assignment in for expression!\n");
            removeLastLocalScope(scope);
            return 7;
        }

        result = assignmentExp(root->left->statements->first->node, scope, parentFunction, strList);
        if (result != 0) {
            removeLastLocalScope(scope);
            return result;
        }
    }
    //for_end_Assign();

    for_suffix(parentFunction);
    removeLastLocalScope(scope);
    return result;
}

/**
 * Main switch to redirect code to other functions
 * @param root Pointer to the node
 * @param scope Pointer to current scope
 * @param parentFunction Name of parent function
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
long statementMainSwitch(SyntaxNode* root, tScope* scope, char* parentFunction, tStringLinkedListItem* strList) {
    enum typeOfNode type = root->type;
    switch(type) {
        case Node_DeclareExpression:
            return declareExp(root, scope, parentFunction, strList);
        case Node_AssignmentExpression:
            return assignmentExp(root, scope, parentFunction, strList);
        case Node_IFExpression:
            return ifExpression(root, scope, parentFunction, strList);
        case Node_ForExpression:
            return forExpression(root, scope, parentFunction, strList);
        case Node_FunctionCallExpression:
            return callFunction(root, scope, parentFunction, strList);
        case Node_ReturnExpression:
            return returnExp(root, scope, parentFunction, strList);

        case Node_IdentifierExpression:
            return identifierExp(root, scope, parentFunction).errCode;

        case Node_BooleanExpression:
        case Node_ParenthezedExpression:
        case Node_BinaryExpression:
        case Node_NumberIntExpression:
        case Node_NumberDoubleExpression:
        case Node_StringExpression:
        case Node_UnaryExpression:
            return evalExpression(root, scope, parentFunction, strList).errCode;

        default:
            return 7;
    }

}


/**
 * Block expression node
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
 * @param strList Pointer to temp string list
 * @return Return code (0 = OK)
 */
long runFunctionExp(SyntaxNode* root, tScope* scope, tStringLinkedListItem* strList){
    char* funcName = root->left->left->token->value;
    tHashTable* table = scope->global->table;
    //definition first
    if (getHashItem(table, funcName) == NULL) {
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
    }
    else if (getHashItem(table, funcName)->declared == false) {  //function was already called, so now we need to check if params and return values are correct
        tFuncItem* func = getHashItem(table, funcName)->func;
        SyntaxNodes *param = root->left->statements != NULL ? root->left->statements->first : NULL;
        int index = 0;
        while (param != NULL) {
            if (index >= func->params_count || getDataTypeFromString(param->node->right->token->value) != func->paramsTypes[index]) {
                //fprintf(stderr, "Function \"%s\" has been called with different parameters!", funcName);
                return 6;
            }

            if (func->params[index] != NULL) {  //memoryleak fix
                free(func->params[index]);
            }
            func->params[index] = malloc(sizeof(char) * (strlen(param->node->left->token->value) + 1));     //need to copy string or it will destroy everything (bad pointer)
            if (func->params[index] == NULL)
                return 99;
            strcpy(func->params[index], param->node->left->token->value);   //update name of fce's parameter

            index++;
            param = param->next;
        }

        if (func->params_count > index) {
            //fprintf(stderr, "Function \"%s\" has been called with more parameters!", funcName);
            return 6;
        }

        if (func->return_count > 0) {
            SyntaxNodes *ret = root->statements != NULL ? root->statements->first : NULL;
            index = 0;
            while (ret != NULL) {
                if (index >= func->return_count || (getDataTypeFromString(ret->node->token->value) != func->return_vals[index] && func->return_vals[index] != TEverything)) {
                    //fprintf(stderr, "Function \"%s\" has been called with different return values!", funcName);
                    return 6;
                }
                if (func->return_vals[index] == TEverything) {
                    func->return_vals[index] = getDataTypeFromString(ret->node->token->value);
                }

                index++;
                ret = ret->next;
            }
            if (index != func->return_count) {
                //fprintf(stderr, "Function \"%s\" has been called with different return values!", funcName);
                return 6;
            }
        }
        else {  //add return values
            SyntaxNodes *ret = root->statements != NULL ? root->statements->first : NULL;
            while (ret != NULL) {
                addReturnTypeToFunc(table, funcName, getDataTypeFromString(ret->node->token->value));
                ret = ret->next;
            }
        }

        getHashItem(table, funcName)->declared = true;  //function is now finally declared
    }
    else {
        //fprintf(stderr, "Function %s is already declared!\n", funcName);
        return 3;
    }

    foundReturn = false;
    tFuncItem * func =  getHashItem(table, funcName)->func;
    if(strcmp(funcName, "main") == 0)
        main_prefix();
    else {
        general_func_prefix(funcName, func); // GENERATE FUNCTION NAME
        func_ret_declar(funcName, func);
    }
    long errCode = blockExp(root->right, scope, funcName, strList);   //run things in the body of the function | also return it's errCode

    if (getHashItem(scope->global->table, funcName)->func->return_count > 0 && foundReturn == false) {
        //fprintf(stderr, "Missing return statement!\n");
        return 6;
    }
    if(strcmp(funcName, "main") == 0)
        main_suffix();
    else{
        general_func_suffix(funcName); // GENERATE CODE END OF FUNCTION
    }
    return errCode;
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
    addFuncToHT(table, "print", true);
    //addParamToFunc(table, "print",)

    //func int2float(i int) (float64)
    addFuncToHT(table, "int2float", true);
    addParamToFunc(table, "int2float", "arg_0", TInt);
    addReturnTypeToFunc(table, "int2float", TDouble);

    //func float2int(f float64) (int)
    addFuncToHT(table, "float2int", true);
    addParamToFunc(table, "float2int", "arg_0", TDouble);
    addReturnTypeToFunc(table, "float2int", TInt);

    //func len(s string) (int)
    addFuncToHT(table, "len", true);
    addParamToFunc(table, "len", "arg_0", TString);
    addReturnTypeToFunc(table, "len", TInt);

    //func substr(s string, i int, n int) (string, int)
    addFuncToHT(table, "substr", true);
    addParamToFunc(table, "substr", "arg_0", TString);
    addParamToFunc(table, "substr", "arg_1", TInt);
    addParamToFunc(table, "substr", "arg_2", TInt);
    addReturnTypeToFunc(table, "substr", TString);
    addReturnTypeToFunc(table, "substr", TInt);

    //func ord(s string, i int) (int, int)
    addFuncToHT(table, "ord", true);
    addParamToFunc(table, "ord", "arg_0", TString);
    addParamToFunc(table, "ord", "arg_1", TInt);
    addReturnTypeToFunc(table, "ord", TInt);
    addReturnTypeToFunc(table, "ord", TInt);

    //func chr(i int) (string, int)
    addFuncToHT(table, "chr", true);
    addParamToFunc(table, "chr", "arg_0", TInt);
    addReturnTypeToFunc(table, "chr", TString);
    addReturnTypeToFunc(table, "chr", TInt);

    bif_inputi();
    bif_inputs();
    bif_inputf();
    bif_chr();
    bif_ord();
    bif_substr();
    bif_lenght();
    bif_int2float();
    bif_float2int();
    return 0;
}

/**
 * Main function of semantic analysis
 * @param root Start node of syntax tree
 * @param scope Global scope
 * @return Return code (0 = OK)
 */
long runSemanticAnalyze(SyntaxNode* root, tScope* scope){
    if(root == NULL){
        return 99;
    }
    disableAssignment = false;

    tStringLinkedListItem* strList = malloc(sizeof(tStringLinkedListItem));
    createList(strList);

    program_start(); // GENCODE!
    addInbuiltFunctions(scope);    //loads inbuilt functions

    if (root->type == Node_Global){
        SyntaxNodes *statement = root->statements != NULL ? root->statements->first : NULL;
        while (statement != NULL){
            if (statement->node->type != Node_FunctionExpression){
                //fprintf(stderr, "%s is outside of any function!\n", statement->node->name);
                return 3;
            }

            long returnCode = runFunctionExp(statement->node, scope, strList);  //evaluate expressions

            if (returnCode != 0) {
                destroyList(strList);
                return returnCode;
            }

            statement = statement->next;
        }
    }

    if (getHashItem(scope->topLocal->table, "main") != NULL) {               /** Checks if main function is declared as it should be */
        tHashItem* mainFunc = getHashItem(scope->topLocal->table, "main");
        if (mainFunc->declared == true && mainFunc->func != NULL && mainFunc->func->params_count == 0 && mainFunc->func->return_count == 0) {

            for(int i = 0; i < scope->global->table->size; i++) {
                for(tHashItem* tmp = scope->global->table->table[i]; tmp != NULL; tmp = tmp->next)  {
                    if (tmp->func != NULL) {
                        if (tmp->declared == false) {
                            //fprintf(stderr, "Function \"%s\" not declared!\n", tmp->id);
                            destroyList(strList);
                            return 3;
                        }
                    }
                }
            }

            destroyList(strList);
            //program_exit(NULL);
            return 0;
        }
        else {
            //fprintf(stderr, "Main function should not have parameters or return value\n");
            destroyList(strList);
            return 6;
        }
    }

    //fprintf(stderr, "Main function not declared!\n");
    destroyList(strList);
    return 3;
}