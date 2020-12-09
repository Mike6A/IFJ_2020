/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz) implemented general functions for gen_code
 * @author Dávid Oravec (xorave06@stud.fit.vutbr.cz) implemented expressions
 */

#include "gen_code.h"

static int scope = 0;
static int if_else_counter = 0;
t_INT_Stack if_else_stack;
t_INT_Stack for_stack;
static int for_counter = 0;
static bool skipDefvarAssignTemp = false;
static int assignC = 0;
static int spec_var = 0;
idList * currentScopeVars = NULL;
static pList gencodeList = {.first = NULL, .last = NULL};
static int errorcodeList = 0;
//PRINT LIST NODE
int getErrorList(){
    return errorcodeList;
}
void initpList(pList *list)
{
    list->first = NULL;
}

TItem getTypeOfExpr(SyntaxNode* root){
    SyntaxNode *current = root;
    if(current != NULL){
        while (current->right != NULL && (  current->type != Node_NumberIntExpression ||
                                            current->type != Node_NumberDoubleExpression ||
                                            current->type != Node_StringExpression)){
            current = current->right;
        }
        if(current->type == Node_NumberIntExpression || current->type == Node_NumberIntToken){
            return TInt;
        }else if(current->type == Node_NumberDoubleExpression || current->type == Node_NumberDoubleToken){
            return TDouble;
        }else if(current->type == Node_StringExpression || current->type == Node_StringToken){
            return TString;
        }
    }
    return TEverything;
}

void addItemTopList(const char* format, ... )
{

    va_list args;
    //init valist for arg_num number of function args
    va_start(args, format);
    //main access to arguments
    size_t needed = vsnprintf(NULL, 0,format, args)+2;
    char * buffer = malloc(needed);
    if(buffer == NULL) {
        errorcodeList = 99;
        va_end(args);
        return;
    }
    va_end(args);
    va_start(args, format);
    vsprintf(buffer, format, args);
    //vprintf(format, args);
    pNode* newGenLine = malloc(sizeof(pNode));
    if(newGenLine == NULL) {
        errorcodeList = 99;
        va_end(args);
        return;
    }
    newGenLine->next = NULL;
    newGenLine->func_defined = true;
    newGenLine->printString = buffer;
    if(gencodeList.first == NULL){
        gencodeList.first = newGenLine;
        gencodeList.last = newGenLine;
    }else{
        gencodeList.last->next = newGenLine;
        gencodeList.last = newGenLine;
    }
    //clean used memory
    va_end(args);

}

void addItemTopListCallFunction(char* funcName, bool funcIsDeclared,bool defvar, const char* format, ... )
{

    va_list args;
    //init valist for arg_num number of function args
    va_start(args, format);
    //main access to arguments
    size_t needed = vsnprintf(NULL, 0,format, args)+2;
    char * buffer = malloc(needed);
    if(buffer == NULL) {
        errorcodeList = 99;
        va_end(args);
        return;
    }
    va_end(args);
    va_start(args, format);
    vsprintf(buffer, format, args);
    //vprintf(format, args);
    pNode* newGenLine = malloc(sizeof(pNode));
    if(newGenLine == NULL) {
        errorcodeList = 99;
        va_end(args);
        return;
    }
    newGenLine->next = NULL;
    newGenLine->func_defined = funcIsDeclared;
    newGenLine->defvarFunc = defvar;
    newGenLine->func_name = funcName;
    newGenLine->printString = buffer;
    if(gencodeList.first == NULL){
        gencodeList.first = newGenLine;
        gencodeList.last = newGenLine;
    }else{
        gencodeList.last->next = newGenLine;
        gencodeList.last = newGenLine;
    }
    //clean used memory
    va_end(args);

}

void deletepList()
{
    pNode* current = gencodeList.first;
    while(current != NULL){
        pNode * tmp = current;
        current = current->next;
        free(tmp->printString);
        free(tmp);
    }
    gencodeList.first = NULL;
    gencodeList.last = NULL;
}

void printAndDeleteGenCode(tScope* scopeVar){
    pNode* current = gencodeList.first;
    char *prevFuncName = NULL;
    int paramNumber = 0;
    bool defvar = false;
    while(current != NULL){
        if(!current->func_defined){
            char* token = NULL;
            int index = 0;
            if(prevFuncName != NULL && strcmp(prevFuncName, current->func_name) != 0 ){
                paramNumber = 0;
            }
            tHashItem *TITEM = getHashItem(scopeVar->global->table, current->func_name);
            tFuncItem *func = NULL;
            if (TITEM != NULL)
                func = TITEM->func;
            if (func != NULL) {
                if(current->printString != NULL) {
                    current->printString = realloc(current->printString, sizeof(char)*(malloc_usable_size(current->printString)+malloc_usable_size(func->params[paramNumber]) + 1));
                    char *buffer = malloc(sizeof(char)*(malloc_usable_size(current->printString)+1));
                    if(buffer == NULL){
                        errorcodeList = 99;
                        return;
                    }
                    token = strtok(current->printString, "@ ");
                    if(token!= NULL && strcmp(token, "MOVE") == 0)
                        defvar = true;
                    if(token!= NULL && strcmp(token, "DEFVAR") == 0)
                        defvar = false;
                    if (buffer != NULL) {
                        strcpy(buffer, "");
                        while (token != NULL) {
                            if (index != 2) {
                                strcat(buffer, token);
                                if (index == 1 || index == 3)
                                    strcat(buffer, "@\0");
                                else
                                    strcat(buffer, " \0");
                            } else {

                                strcat(buffer, func->params[paramNumber]);
                                strcat(buffer, token);
                                strcat(buffer, " \0");
                            }
                            ++index;
                            token = strtok(NULL, "@ ");
                        }
                        if(defvar){
                            paramNumber++;
                        }
                        if(paramNumber >= func->params_count)
                            paramNumber = 0;
                        prevFuncName = current->func_name;
                        free(current->printString);
                        current->printString = buffer;
                    }
                }
            }
        }

        pNode * tmp = current;
        current = current->next;
        if(tmp->printString != NULL) {
            printf("%s", tmp->printString);
            free(tmp->printString);
        }
        free(tmp);
    }
    gencodeList.first = NULL;
    gencodeList.last = NULL;
}


//ID LIST NODE

void initIdList(idList* list){
    list->first = NULL;
    list->last = NULL;
    list->prevScope = NULL;
}

void deleteIdListScope(idList** list){
    if(list != NULL && (*list) != NULL){
        idListNode * current = (*list)->last;
        while(current != NULL){
            idListNode * tmp = current;
            current = current->prev;
            free(tmp->name);
            tmp->name = NULL;
            free(tmp);
            tmp = NULL;
        }
        (*list)->first = NULL;
        (*list)->last = NULL;
        (*list)->scope = -1;
        idList* tmpIdList = (*list);
        (*list) = (*list)->prevScope;
        tmpIdList->prevScope = NULL;
        free(tmpIdList);
        tmpIdList = NULL;
    }
    if(scope > 0)
        scope--;
}

void deleteIdList(idList** list){
    if(list != NULL){
        while((*list) != NULL){
            deleteIdListScope(list);
        }
    }
}
void deleteIdListGlobal(){
    while(currentScopeVars != NULL){
        deleteIdListScope(&currentScopeVars);
    }
}
void createNewIdListScope(idList** list){
    if((*list) != NULL)
        scope++;
    idList* newScopeList = malloc(sizeof(idList));
    if(newScopeList == NULL){
        errorcodeList = 99;
        return;
    }
    newScopeList->scope = scope;
    newScopeList->prevScope = (*list);
    newScopeList->first = NULL;
    newScopeList->last = NULL;
    (*list) = newScopeList;
}
void addItemToIdList(idList** list, char* name){
    if(list != NULL && (*list) != NULL){
        idListNode* newNode = malloc(sizeof(idListNode));
        if(newNode == NULL){
            errorcodeList = 99;
            return;
        }
        char * newName = malloc(sizeof(char) * strlen(name)+1);
        if(newName == NULL){
            free(newNode);
            errorcodeList = 99;
            return;
        }
        strcpy(newName, name);
        newNode->name = newName;
        newNode->prev = (*list)->last;
        newNode->next = NULL;
        if((*list)->last != NULL)
            (*list)->last->next = newNode;
        (*list)->last = newNode;
    }
}
int getLastScopeOfIdInList(idList* list, char* item){
    idList *currentScope = list;
    while (currentScope != NULL){
        idListNode *currentScopeNode = currentScope->last;
        while(currentScopeNode != NULL){
            if(strcmp(currentScopeNode->name, item) == 0){
                return currentScope->scope;
            }
            currentScopeNode = currentScopeNode->prev;
        }

        currentScope = currentScope->prevScope;
    }
    return -1;
}


void parse_str(char *str)
{

    int old_len = strlen(str);
    int ord = 0;
    char hex_arr[3];

    for(int i=1; i< old_len-1;i++)
    {

        ord = str[i];
        if((ord>=0 && ord<=32)||(ord == 35))
        {
            addItemTopList("\\%d%d",0,ord);
        }
        else if(ord == 92)
        {
                            
            switch(str[i+1])
            {
                                
                case 'n': 
                        addItemTopList("\\010");
                        break;

                case 't':
                        addItemTopList("\\009");
                        break;

                case '"':
                        addItemTopList("\\034");
                        break;

                case '\\':
                        addItemTopList("\\092");
                        break;
                //hex
                case 'x':
                        hex_arr[0] = str[i+2];
                        hex_arr[1] = str[i+3];
                        hex_arr[2] = '\0';
                        ord = hex_to_dec(hex_arr);
                        if(ord > 99){
                            addItemTopList("\\%d",ord);
                        }else if(ord > 9){
                            addItemTopList("\\0%d",ord);
                        }else if(ord >= 0){
                            addItemTopList("\\00%d",ord);
                        }


                        i+=2;
                                            
            }
        ++i;
        }
        else
        {
            addItemTopList("%c",str[i]);
        }
                           
    }

}

///---------BUILT-IN FUNCTIONS-----------------

/// built-in string functions

void bif_lenght()
{
    addItemTopList(0);
    addItemTopList("# func len(s string) (int)\n");
    addItemTopList("LABEL _len\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@len_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@len_ret_0_%d int@0\n", scope);
    addItemTopList("STRLEN LF@len_ret_0_%d LF@arg_0\n", scope);
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");

}

void bif_substr()
{
    addItemTopList("# func substr(s string,i int,n int) (string, int)\n");
    addItemTopList("LABEL _substr\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@substr_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@substr_ret_0_%d string@\n", scope);
    addItemTopList("DEFVAR LF@substr_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@substr_ret_1_%d int@0\n", scope);
    addItemTopList("ADD LF@arg_1 LF@arg_1 int@1\n");
    addItemTopList("DEFVAR LF@in_string\n");
    addItemTopList("DEFVAR LF@str_len\n");
    addItemTopList("DEFVAR LF@substr_len\n");
    addItemTopList("STRLEN LF@str_len LF@arg_0\n");
    addItemTopList("MOVE LF@substr_len LF@str_len\n");
    addItemTopList("MOVE LF@in_string bool@true\n");
    addItemTopList("JUMPIFEQ _func_empty_substr LF@arg_2 int@0\n");
    addItemTopList("LT LF@in_string LF@str_len int@1\n");
    addItemTopList("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    addItemTopList("LT LF@in_string LF@arg_1 int@0\n");
    addItemTopList("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    addItemTopList("GT LF@in_string LF@arg_1 LF@str_len\n");
    addItemTopList("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    addItemTopList("LT LF@in_string LF@arg_2 int@0\n");
    addItemTopList("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    addItemTopList("SUB LF@substr_len LF@substr_len LF@arg_1\n");
    addItemTopList("GT LF@in_string LF@arg_2 LF@substr_len\n");
    addItemTopList("JUMPIFNEQ _func_continue_substr LF@in_string bool@true\n");
    addItemTopList("ADD LF@substr_len LF@substr_len int@1\n");
    addItemTopList("MOVE LF@arg_2 LF@substr_len\n");
    addItemTopList("LABEL _func_continue_substr\n");
    addItemTopList("DEFVAR LF@index\n");
    addItemTopList("MOVE LF@index LF@arg_1\n");
    addItemTopList("SUB LF@index LF@index int@1\n");
    addItemTopList("DEFVAR LF@char_on_index\n");
    addItemTopList("DEFVAR LF@loop_counter\n");
    addItemTopList("MOVE LF@loop_counter int@0\n");
    addItemTopList("LABEL _func_loop_substr\n");
    addItemTopList("GETCHAR LF@char_on_index LF@arg_0 LF@index\n");
    addItemTopList("CONCAT LF@substr_ret_0_%d LF@substr_ret_0_%d LF@char_on_index\n", scope, scope);
    addItemTopList("ADD LF@index LF@index int@1\n");
    addItemTopList("ADD LF@loop_counter LF@loop_counter int@1\n");
    addItemTopList("JUMPIFNEQ _func_loop_substr LF@loop_counter LF@arg_2\n");
    addItemTopList("JUMP _func_end_substr\n");
    addItemTopList("LABEL _func_empty_substr\n");
    addItemTopList("MOVE LF@substr_ret_0_%d string@\n",scope);
    addItemTopList("JUMP _func_end_substr\n");
    addItemTopList("LABEL _func_error_substr\n");
    addItemTopList("MOVE LF@substr_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_substr\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

void bif_ord()
{
    addItemTopList("# func ord(s string,i int) (int, int)\n");
    addItemTopList("LABEL _ord\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@ord_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@ord_ret_0_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@ord_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@ord_ret_1_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@in_string\n");
    addItemTopList("MOVE LF@in_string bool@true\n");
    addItemTopList("LT LF@in_string LF@arg_1 int@0\n");
    addItemTopList("JUMPIFEQ _func_error_ord LF@in_string bool@true\n");
    addItemTopList("DEFVAR LF@max_int_value\n");
    addItemTopList("STRLEN LF@max_int_value LF@arg_0\n");
    addItemTopList("SUB LF@max_int_value LF@max_int_value int@1\n");
    addItemTopList("GT LF@in_string LF@arg_1 LF@max_int_value\n");
    addItemTopList("JUMPIFEQ _func_error_ord LF@in_string bool@true\n" );
    addItemTopList("STRI2INT LF@ord_ret_0_%d LF@arg_0 LF@arg_1\n", scope );
    addItemTopList("JUMP _func_end_ord\n");
    addItemTopList("LABEL _func_error_ord\n");
    addItemTopList("MOVE LF@ord_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_ord\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");

}

void bif_chr()
{
    addItemTopList("# func chr(i int) (string, int)\n");
    addItemTopList("LABEL _chr\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@chr_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@chr_ret_0_%d string@\n", scope);
    addItemTopList("DEFVAR LF@chr_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@chr_ret_1_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@in_string\n");
    addItemTopList("MOVE LF@in_string bool@true\n");
    addItemTopList("LT LF@in_string LF@arg_0 int@0\n");
    addItemTopList("JUMPIFEQ _func_error_chr LF@in_string bool@true\n");
    addItemTopList("GT LF@in_string LF@arg_0 int@255\n");
    addItemTopList("JUMPIFEQ _func_error_chr LF@in_string bool@true\n");
    addItemTopList("INT2CHAR LF@chr_ret_0_%d LF@arg_0\n", scope);
    addItemTopList("JUMP _func_end_chr\n");
    addItemTopList("LABEL _func_error_chr\n");
    addItemTopList("MOVE LF@chr_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_chr\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

///build-in functions for data type conversion

void bif_int2float()
{
    addItemTopList("# func int2float(i int) (float64 int)\n");
    addItemTopList("LABEL _int2float\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@int2float_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@int2float_ret_0_%d float@0x1.2666666666666p+0\n",scope);
    addItemTopList("INT2FLOAT LF@int2float_ret_0_%d LF@arg_0\n",scope);
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

void bif_float2int()
{
    addItemTopList("# func float2int(f float64) (int int)\n");
    addItemTopList("LABEL _float2int\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@float2int_ret_0_%d\n", scope);
    addItemTopList("MOVE LF@float2int_ret_0_%d int@0\n",scope);
    addItemTopList("FLOAT2INT LF@float2int_ret_0_%d LF@arg_0\n",scope);
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

///built-in function + parameters for value output

int hex_to_dec(char hex[]) 
{    
    int len = strlen(hex); 
    int base = 1; 
    int dec_val = 0; 
    for (int i=len-1; i>=0; i--) 
    {    
        
        if (hex[i]>='0' && hex[i]<='9') 
        { 
            dec_val += (hex[i] - 48)*base; 
                  
            base = base * 16; 
        } else if (hex[i]>='A' && hex[i]<='F') 
        { 
            dec_val += (hex[i] - 55)*base; 
          
            base = base*16; 
        } else if (hex[i]>='a' && hex[i]<='f')
        {
            dec_val += (hex[i] - 55-32)*base;

            base = base*16;
        }
    } 
      
    return dec_val; 
} 

void bif_print(SyntaxNodes* my_statement)
{

    SyntaxNodes* current_statement = my_statement != NULL? my_statement->first: NULL;
    static int c = 0;
    static int ord = 0;
    static char hex_arr[3];
    double f = 0.0;
    while(current_statement != NULL)
    {
        if(current_statement->node != NULL) {
            int whichScope = -1;
            switch (current_statement->node->type) {
                case Node_NumberIntExpression:
                    addItemTopList("WRITE int@%s\n", current_statement->node->right->token->value);
                    break;
                case Node_NumberDoubleExpression:
                    f = atof(current_statement->node->right->token->value);
                    addItemTopList("WRITE float@%a\n", f);
                    break;
                case Node_StringExpression:     
                    for(int i=1; i< strlen(current_statement->node->right->token->value)-1;i++)
                    {

                        ord = current_statement->node->right->token->value[i];
                        if((ord>=0 && ord<=32)||(ord == 35))
                        {
                            addItemTopList("WRITE string@\\%d%d\n",0,ord);
                        }
                            
                        else if(ord == 92)
                        {
                            
                            switch(current_statement->node->right->token->value[i+1])
                            {
                                
                                case 'n': 
                                        addItemTopList("WRITE string@\\010\n");
                                        break;

                                case 't':
                                        addItemTopList("WRITE string@\\009\n");
                                        break;

                                case '"':
                                        addItemTopList("WRITE string@\\034\n");
                                        break;
                                case '\\':
                                        addItemTopList("WRITE string@\\092\n");
                                        break;
                                //hex
                                case 'x':
                                        hex_arr[0] = current_statement->node->right->token->value[i+2];
                                        hex_arr[1] = current_statement->node->right->token->value[i+3];
                                        hex_arr[2] = '\0';
                                        ord = hex_to_dec(hex_arr);
                                        addItemTopList("WRITE string@%c\n",ord);
                                        i+=2;
                                            
                            }
                            ++i;
                        }
                        else
                        {
                            addItemTopList("WRITE string@%c\n",current_statement->node->right->token->value[i]);
                        }
                           
                    }

                    break;
                case Node_IdentifierExpression:
                    whichScope = getLastScopeOfIdInList(currentScopeVars, current_statement->node->right->token->value);
                    addItemTopList("WRITE LF@%s_%d\n",current_statement->node->right->token->value , whichScope);
                    break;
                case Node_UnaryExpression:

                    addItemTopList("DEFVAR LF@%s\n", "tmpWriteExprUnary");
                    char temp1[15];
                    sprintf(temp1, "__WLEFT__%d", c);

                    addItemTopList("DEFVAR LF@%s\n", temp1);
                    char temp2[15];
                    sprintf(temp2, "__WRIGHT__%d", c++);
                    addItemTopList("DEFVAR LF@%s\n", temp2);

                    int type = getTypeOfExpr(current_statement->node); //@TODO
                    struct genExpr unary = GenParseExpr(current_statement->node, "tmpWriteExprUnary", temp1, temp2,
                                                        get_var_type(type));
                    addItemTopList("WRITE %s@%s%s\n", unary.constant ? unary.type : "LF" , unary.sign ? "-" : "", unary.value);
                    break;
                case Node_BinaryExpression:
                    addItemTopList("DEFVAR LF@%s\n", "tmpWriteExprBin");
                    char temp3[15];
                    sprintf(temp3, "__WLEFT__%d", c);

                    addItemTopList("DEFVAR LF@%s\n", temp3);
                    char temp4[15];
                    sprintf(temp4, "__WRIGHT__%d", c++);
                    addItemTopList("DEFVAR LF@%s\n", temp4);
                    int typeBin = getTypeOfExpr(current_statement->node); //@TODO
                    struct genExpr binary = GenParseExpr(current_statement->node, "tmpWriteExprBin", temp3, temp4, get_var_type(typeBin));
                    addItemTopList("WRITE %s@%s%s\n", binary.constant ? binary.type : "LF", binary.sign ? "-" : "", binary.value);
                    break;
                default:
                    break;
            }
        }
        current_statement = current_statement->next;

    }
    
}

///built-in functions for read literates and print terms

void bif_inputs()
{
    addItemTopList("# func inputs() (string,int)\n");
    addItemTopList("LABEL _inputs\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@inputs_ret_0_%d\n", scope);
    addItemTopList("DEFVAR LF@inputs_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@inputs_ret_1_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@input_type\n");
    addItemTopList("READ LF@inputs_ret_0_%d string\n", scope);
    addItemTopList("TYPE LF@input_type LF@inputs_ret_0_%d\n", scope);
    addItemTopList("JUMPIFNEQ _func_err_inputs LF@input_type string@string\n");
    addItemTopList("JUMP _func_end_inputs\n");
    addItemTopList("LABEL _func_err_inputs\n");
    addItemTopList("MOVE LF@inputs_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_inputs\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

void bif_inputi()
{
    addItemTopList("# func inputi() (int,int)\n");
    addItemTopList("LABEL _inputi\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@inputi_ret_0_%d\n", scope);
    addItemTopList("DEFVAR LF@inputi_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@inputi_ret_1_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@input_type\n");
    addItemTopList("READ LF@inputi_ret_0_%d int\n", scope);
    addItemTopList("TYPE LF@input_type LF@inputi_ret_0_%d\n", scope);
    addItemTopList("JUMPIFNEQ _func_err_inputi LF@input_type string@int\n");
    addItemTopList("JUMP _func_end_inputi\n");
    addItemTopList("LABEL _func_err_inputi\n");
    addItemTopList("MOVE LF@inputi_ret_0_%d int@0\n", scope);
    addItemTopList("MOVE LF@inputi_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_inputi\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

void bif_inputf()
{
    addItemTopList("# func inputf() (float64,int)\n");
    addItemTopList("LABEL _inputf\n");
    addItemTopList("PUSHFRAME\n");
    addItemTopList("DEFVAR LF@inputf_ret_0_%d\n", scope);
    addItemTopList("DEFVAR LF@inputf_ret_1_%d\n", scope);
    addItemTopList("MOVE LF@inputf_ret_1_%d int@0\n", scope);
    addItemTopList("DEFVAR LF@input_type\n");
    addItemTopList("READ LF@inputf_ret_0_%d float\n", scope);
    addItemTopList("TYPE LF@input_type LF@inputf_ret_0_%d\n", scope);
    addItemTopList("JUMPIFNEQ _func_err_inputf LF@input_type string@float\n");
    addItemTopList("JUMP _func_end_inputf\n");
    addItemTopList("LABEL _func_err_inputf\n");
    addItemTopList("MOVE LF@inputf_ret_0_%d float@0x0p+0\n", scope);
    addItemTopList("MOVE LF@inputf_ret_1_%d int@1\n", scope);
    addItemTopList("LABEL _func_end_inputf\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
}

///---------GENERATE PREFIX & SUFFIX FOR ALL FUNCTIONS-----------------

void program_start()
{
    init_INT_Stack(&for_stack);
    init_INT_Stack(&if_else_stack);
    addItemTopList("# START OF GEN_CODE\n");
    addItemTopList(".IFJcode20\n");
    addItemTopList("JUMP _main\n");

}

void main_prefix()
{

    addItemTopList("# MAIN FUNCTION\n");
    addItemTopList("LABEL _main\n");
    addItemTopList("CREATEFRAME\n");
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);

}

void main_suffix()
{

    addItemTopList("# END OF MAIN FUNCTION\n");
    addItemTopList("POPFRAME\n");
    addItemTopList("CLEARS\n");
    addItemTopList("EXIT int@0\n");
    deleteIdListScope(&currentScopeVars);

}

void general_func_call(char *func_name)
{
    addItemTopList("CALL _%s\n",func_name);

}

void general_func_prefix(char *func_name, tFuncItem *func)
{

    addItemTopList("# START OF THE FUNCTION %s\n",func_name);
    addItemTopList("LABEL _%s\n",func_name);
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    for(int i = 0; i < func->params_count; ++i){
        addItemToIdList(&currentScopeVars, func->params[i]);
    }


}

void general_func_suffix(char *func_name)
{

    addItemTopList("LABEL _%s_ret\n", func_name);
    addItemTopList("POPFRAME\n");
    addItemTopList("RETURN\n");
    deleteIdListScope(&currentScopeVars);
    deleteIdListScope(&currentScopeVars);
}

void declared_vars_default_init(TItem type)
{

    switch(type)
    {

        case TInt:
                addItemTopList("int@0\n");
                break;

        case TDouble: 
                addItemTopList("float@0x0p+0\n");
                break;

        case TString:
                addItemTopList("string@\n");
                break;

        case TBool:
                addItemTopList("bool@false\n");
                break;

        default:
                break;
        
    }

}

char* get_var_type(TItem type)
{

    switch(type)
    {

        case TInt:
                return "int";
               

        case TDouble: 
                return "float";
                

        case TString:
                return "string";
                

        case TBool:
                return "bool";
                
        default:
                return "nil";
    }

}

void func_args_TF_declar(char *func_name, tHashItem *funcItem, SyntaxNodes* paramValues)
{

    static int called = 0;
    tFuncItem *func = funcItem->func;
    SyntaxNodes * currentParam = paramValues;
    //have to create before args pass(for non args func too)
    addItemTopList("CREATEFRAME\n");
    if (paramValues == NULL){
        return;
    }
        addItemTopList("# CREATE VARS FOR %s's ARGS\n",func_name);
        for(int i = 0; i < func->params_count ; i++)
        {
            char paramId[64];
            sprintf(paramId, "%s__P__%d_%d",func_name, i,called);

            addItemTopList("DEFVAR LF@%s\n",paramId);
            char temp1[64];
            sprintf(temp1, "%s__LEFT__%d_%d",func_name, i,called);

            addItemTopList("DEFVAR LF@%s\n", temp1);
            char temp2[64];
            sprintf(temp2, "%s__RIGHT__%d_%d",func_name, i,called++);
            addItemTopList("DEFVAR LF@%s\n", temp2);
            struct genExpr param = GenParseExpr(currentParam->node, paramId, temp1, temp2, get_var_type(func->paramsTypes[i]));
            if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                addItemTopList("DEFVAR TF@%s\n",func->params[i]);
            } else{
                addItemTopListCallFunction(func_name,funcItem->declared,true,"DEFVAR TF@%s_%d\n",func->params[i],0);
            }
            if(strcmp(param.type, "string") == 0){
                if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                    addItemTopList("MOVE TF@%s %s@%s",func->params[i], param.constant? param.type : "LF", param.sign ? "-":"");
                }else
                    addItemTopListCallFunction(func_name,funcItem->declared,false,"MOVE TF@%s_%d %s@%s",func->params[i], 0, param.constant? param.type : "LF", param.sign ? "-":"");
                if(!param.constant || strcmp(param.type, "LF") == 0)
                    addItemTopList("%s", param.value);
                else
                    parse_str(param.value);
                addItemTopList("\n");
            } else{
            if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                addItemTopList("MOVE TF@%s %s@%s%s\n",func->params[i], param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            }else
                addItemTopListCallFunction(func_name,funcItem->declared,false,"MOVE TF@%s_%d %s@%s%s\n",func->params[i], 0, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            }
            //declared_vars_default_init(func->paramsTypes[i]);
            currentParam = currentParam->next;
            if(param.alocated)
                free(param.value);
        }

        addItemTopList("# ALL VARS FOR ARGS HAS BEEN CREATED\n");
      
}

void func_ret_to_LF(char *func_name, tFuncItem *func, SyntaxNodes* dest)
{
    SyntaxNodes* currentDest = dest;
    //addItemTopList(">>>>>>>>>>>>>%s\n",currentDest->node->name);
    for(int i = 0; i<func->return_count; i++)
    {
        if(strcmp(currentDest->node->left->token->value, "_") != 0){
            int whichScope = getLastScopeOfIdInList(currentScopeVars, currentDest->node->left->token->value);
            int countOfI = 1;
            int tmpI = i;
            while(tmpI > 9){
                ++countOfI;
                tmpI /= 10;
            }
            char* retId = malloc(sizeof(char)*strlen(func_name) + 5 +countOfI+1);
            sprintf(retId, "%s_ret_%d", func_name, i);
            int whichScopeRet = 0;//getLastScopeOfIdInList(currentScopeVars, retId);
            addItemTopList("MOVE LF@%s_%d TF@%s_%d\n",currentDest->node->left->token->value,whichScope, retId, whichScopeRet);
            free(retId);
        }
        currentDest = currentDest->next;
    }

}

void func_ret_declar(char *func_name, tFuncItem *func)
{

    addItemTopList("# DECLA VALUES FOR %s's RETURNS\n",func_name);
    for(int i = 0; i<func->return_count; i++)
    {
        int countOfI = 1;
        int tmpI = i;
        while(tmpI > 9){
            ++countOfI;
            tmpI /= 10;
        }
        char* retId = malloc(sizeof(char)*strlen(func_name) + 5 +countOfI+1);
        sprintf(retId, "%s_ret_%d",func_name, i);
        addItemToIdList(&currentScopeVars, retId);
        int whichScope = getLastScopeOfIdInList(currentScopeVars, retId);
        addItemTopList("DEFVAR LF@%s_%d\n",retId, whichScope);
        addItemTopList("MOVE LF@%s_%d ",retId,scope);
        free(retId);
        declared_vars_default_init(func->return_vals[i]);

    }
    createNewIdListScope(&currentScopeVars);
    addItemTopList("# ALL VARS FOR RETURNS HAS BEEN DECLARED\n");
    
}

void func_ret_get_value(char *func_name, tFuncItem *func, SyntaxNodes* retValues)
{

    SyntaxNodes * currentRet = retValues;
    //only after func_ret_declar

        addItemTopList("# MOVE VALUES FOR %s's RETURNS\n",func_name);
        for(int i = 0; i<func->return_count; i++)
        {
            
            char retId[64];
            sprintf(retId, "%s__P__%d",func_name, i);

            addItemTopList("DEFVAR LF@%s\n",retId);
            char temp1[64];
            sprintf(temp1, "%s__LEFT__%d",func_name, i);

            addItemTopList("DEFVAR LF@%s\n", temp1);
            char temp2[64];
            sprintf(temp2, "%s__RIGHT__%d",func_name, i);
            addItemTopList("DEFVAR LF@%s\n", temp2);
            struct genExpr param = GenParseExpr(currentRet->node, retId, temp1, temp2, get_var_type(func->return_vals[i]));
            int countOfI = 1;
            int tmpI = i;
            while(tmpI > 9){
                ++countOfI;
                tmpI /= 10;
            }
            char* retValueId = malloc(sizeof(char)*strlen(func_name) + 5 +countOfI+1);
            sprintf(retValueId, "%s_ret_%d",func_name, i);
            int whichRetValueScope = getLastScopeOfIdInList(currentScopeVars, retValueId);
            if(strcmp(param.type, "string") == 0){
                addItemTopList("MOVE LF@%s_%d %s@%s",retValueId,whichRetValueScope, param.constant? param.type : "LF", param.sign ? "-":"");
                if(!param.constant || strcmp(param.type, "LF") == 0)
                    addItemTopList("%s", param.value);
                else
                    parse_str(param.value);
                addItemTopList("\n");
            } else
                addItemTopList("MOVE LF@%s_%d %s@%s%s\n",retValueId,whichRetValueScope, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            //addItemTopList("MOVE LF@%s_ret_%d %s@%s%s\n",func_name,i, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            free(retValueId);
            currentRet = currentRet->next;
            if(param.alocated)
                free(param.value);
        }
        for(int x = scope-1; x > 0; --x){
            addItemTopList("POPFRAME\n");
            all_vars_after_new_scope(x);
        }
        addItemTopList("JUMP _%s_ret\n", func_name);
        addItemTopList("# ALL VARS FOR RETURNS HAS BEEN INITTED\n");
    
}

///----------------FUNC FOR VARIABLES NAD EXPR-------------

struct genExpr GenParseExpr(SyntaxNode* root, char* assignTo, char* left, char* right, char* type){

    static bool current_unary = true;//true == plus, false == minus
    static bool unary_before = true;//true == plus, false == minus
    struct genExpr rightTemp = {.value = right, .type = type, .sign = false, .constant = false, .alocated = false};
    struct genExpr leftTemp = {.value = left, .type = type, .sign = false, .constant = false, .alocated = false};

    if(root->type == Node_NumberIntExpression || root->type == Node_NumberDoubleExpression || root->type == Node_StringExpression || root->type == Node_IdentifierExpression) {

        struct genExpr test;
        if (root->type == Node_IdentifierExpression){
            int countScopeLen = 1;
            int whichScope = getLastScopeOfIdInList(currentScopeVars, root->right->token->value);
            int tmpscope = whichScope;
            while(tmpscope > 9){
                ++countScopeLen;
                tmpscope /= 10;
            }
            char *identifier = malloc(sizeof(char)*strlen(root->right->token->value)+1+countScopeLen+2);
            test.alocated = true;
            sprintf(identifier, "%s_%d", root->right->token->value, whichScope);
            test.value = identifier;
        }else {
            test.alocated = false;
            test.value = root->right->token->value;
        }
        test.type = root->type == Node_IdentifierExpression ? "LF" : root->type == Node_NumberIntExpression ? "int" : root->type == Node_NumberDoubleExpression ? "float": "string";

        if(strcmp(test.type, "float") == 0){
            double f = atof(test.value);

            size_t needed = snprintf(NULL, 0,"%a", f)+1;
            test.value = malloc(needed);

            sprintf(test.value, "%a", f); // DANGER!!!!!!
            test.alocated = true;
            //test.value = newVal;
        }
        test.constant = true;
        if(!current_unary) {
            current_unary = true;
            unary_before = true;
            test.sign = true;
            return test;
        }else {
            test.sign = false;
            return test;
        }
    }
    else if(root->type == Node_BinaryExpression) {
        bool changeSign = false;
        if (!current_unary) {
            changeSign = true;
            current_unary = true;
            unary_before = true;
        }
        leftTemp = GenParseExpr(root->left, leftTemp.value, leftTemp.value, rightTemp.value, type);
        rightTemp = GenParseExpr(root->right, rightTemp.value, leftTemp.value, rightTemp.value, type);

        switch (root->token->type) {
            case tokenType_PLUS:
                if(strcmp(type, "string") == 0){
                    addItemTopList("CONCAT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                }else {
                    addItemTopList("ADD LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            case tokenType_MINUS:
                addItemTopList("SUB LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_MUL:
                addItemTopList("MUL LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_DIV:
                //addItemTopList("\n"); // IF rightTemp.value != 0 then jump on LABEL
                if((strcmp(rightTemp.value,"0") == 0)||(strcmp(rightTemp.value,"0.0") == 0))
                {
                    addItemTopList("DPRINT %s", "divide by zero\n");
                    addItemTopList("EXIT int@9\n");
                }
                if (strcmp(type, "int") == 0) {
                    addItemTopList("IDIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                } else if (strcmp(type, "float") == 0) {
                    addItemTopList("DIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            default:
                break;
        }
        if(changeSign){
            if (strcmp(type, "int") == 0) {
                addItemTopList("MUL LF@%s %s@%s %s@%s\n", assignTo, "LF", assignTo, "int", "-1");
            }else if (strcmp(type, "float") == 0) {
                addItemTopList("MUL LF@%s %s@%s %s@%s\n", assignTo, "LF", assignTo, "float", "-1.0");
            }
            changeSign = false;
        }
        if(leftTemp.alocated){
            free(leftTemp.value);
            leftTemp.alocated = false;
        }
        if(rightTemp.alocated){
            free(rightTemp.value);
            rightTemp.alocated = false;
        }
        struct genExpr done = {.value = assignTo, .type = type, .sign = false};

        return done;
    }else if(root->type == Node_UnaryExpression){
        current_unary = root->token->type == tokenType_PLUS;
        if(current_unary != unary_before)
            current_unary = false;
        else
            current_unary = true;
        unary_before = current_unary;
        return GenParseExpr(root->right, assignTo,left, right, type);
    }else if(root->type == Node_BooleanExpression) {
        leftTemp = GenParseExpr(root->left, leftTemp.value, leftTemp.value, rightTemp.value, type);
        rightTemp = GenParseExpr(root->right, rightTemp.value, leftTemp.value, rightTemp.value, type);
        switch (root->token->type) {
            case tokenType_EQ:
                if(strcmp(type, "string") == 0){
                    addItemTopList("EQ LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                }else {
                    addItemTopList("EQ LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                    }
                break;
            case tokenType_GE:
                if(strcmp(type, "string") == 0){
                    addItemTopList("GT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);
                addItemTopList("EQ LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                }else {
                    addItemTopList("GT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                    addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);

                        addItemTopList("EQ LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                        leftTemp.sign ? "-" : "", leftTemp.value,
                        rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);      
                        addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                }
                ++spec_var;
                break;
            case tokenType_LE:
                if(strcmp(type, "string") == 0){
                    addItemTopList("LT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");

                     addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);
                    addItemTopList("EQ LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                    addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                }else {
                    addItemTopList("LT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                    addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);

                        addItemTopList("EQ LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                        leftTemp.sign ? "-" : "", leftTemp.value,
                        rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);      
                        addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                }
                ++spec_var;
                break;
            case tokenType_NEQ:
                if(strcmp(type, "string") == 0){
                    addItemTopList("EQ LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");

                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                    addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);
                    addItemTopList("MOVE LF@%s bool@true\n",assignTo);
                    addItemTopList("JUMP _SPEC_VAR_END_FINAL_%d\n",spec_var);
                    addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                    addItemTopList("MOVE LF@%s bool@false\n",assignTo);
                    addItemTopList("LABEL _SPEC_VAR_END_FINAL_%d\n",spec_var);
                    
                }else {
                    addItemTopList("EQ LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                    addItemTopList("JUMPIFEQ _SPEC_VAR_END_%d LF@%s bool@true\n",spec_var,assignTo);
                    addItemTopList("MOVE LF@%s bool@true\n",assignTo);
                    addItemTopList("JUMP _SPEC_VAR_END_FINAL_%d\n",spec_var);
                    addItemTopList("LABEL _SPEC_VAR_END_%d\n",spec_var);
                    addItemTopList("MOVE LF@%s bool@false\n",assignTo);
                    addItemTopList("LABEL _SPEC_VAR_END_FINAL_%d\n",spec_var);
                    
                }
                ++spec_var;
                break;
            case tokenType_GREATER:
                if(strcmp(type, "string") == 0){
                    addItemTopList("GT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");

                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                }else {
                    addItemTopList("GT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            case tokenType_LESS:
                if(strcmp(type, "string") == 0){
                    addItemTopList("LT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    if(!leftTemp.constant || strcmp(leftTemp.type, "LF") == 0)
                        addItemTopList("%s", leftTemp.value);
                    else
                        parse_str(leftTemp.value);
                    addItemTopList(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    if(!rightTemp.constant || strcmp(rightTemp.type, "LF") == 0)
                        addItemTopList("%s", rightTemp.value);
                    else
                        parse_str(rightTemp.value);
                    addItemTopList("\n");
                }else {
                    addItemTopList("LT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            default:
                break;
        }
        if(leftTemp.alocated){
            free(leftTemp.value);
            leftTemp.alocated = false;
        }
        if(rightTemp.alocated){
            free(rightTemp.value);
            rightTemp.alocated = false;
        }
        struct genExpr done = {.value = assignTo, .type = type, .sign = false};
        return done;
    }
    struct genExpr danger = {.value = "", .type = "", .sign = false};
    return danger;
}

void vars_default_declar_init(SyntaxNode *root, tHashItem *item)
{
    addItemTopList("# DECLARE AND DEFAULT_INIT VAR %s\n",item->id);
    static int c = 0;
    char temp1[256];
    sprintf(temp1, "__DLEFT__%d", c);

    addItemTopList("DEFVAR LF@%s\n", temp1);
    char temp2[256];
    sprintf(temp2, "__DRIGHT__%d", c++);
    addItemTopList("DEFVAR LF@%s\n", temp2);
    int countScopeLen = 1;
    int tmpscope = scope;
    while(tmpscope > 9){
        ++countScopeLen;
        tmpscope /= 10;
    }
    char *identific = malloc(sizeof(char)*strlen(item->id)+1+countScopeLen+2 );
    sprintf(identific, "%s_%d", item->id, scope);
    addItemToIdList(&currentScopeVars, item->id);
    addItemTopList("DEFVAR LF@%s\n",identific);
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(item->type));
    if(strcmp(tmp.type, "string") == 0){
        addItemTopList("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        if(!tmp.constant || strcmp(tmp.type, "LF") == 0)
            addItemTopList("%s", tmp.value);
        else
            parse_str(tmp.value);
        addItemTopList("\n");
    } else
        addItemTopList("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);
    if(tmp.alocated){
        free(tmp.value);
    }
    free(identific);
    //GenParseExpr(root,assignTo,right,left));

}

// only use after vars_default_declar_init!!
void vars_set_new_value(SyntaxNode *root, tHashItem *item)
{
    char temp1[256];
    sprintf(temp1, "__ALEFT__%d", assignC);
    char temp2[256];
    sprintf(temp2, "__ARIGHT__%d", assignC++);
    if(!skipDefvarAssignTemp){
        addItemTopList("DEFVAR LF@%s\n", temp1);
        addItemTopList("DEFVAR LF@%s\n", temp2);
    }

    //addItemTopList("DEFVAR LF@%s\n",item->id);
    int countScopeLen = 1;
    int whichScope = getLastScopeOfIdInList(currentScopeVars, item->id);
    int tmpScope = whichScope;
    while(tmpScope > 9){
        ++countScopeLen;
        tmpScope /= 10;
    }
    char *identific = malloc(sizeof(char)*strlen(item->id)+1+countScopeLen+1);
    sprintf(identific, "%s_%d", item->id, tmpScope);
    addItemTopList("# RE-SET VALUE FOR VAR %s\n",item->id);
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(item->type));
    if(strcmp(tmp.type, "string") == 0){
        addItemTopList("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        if(!tmp.constant || strcmp(tmp.type, "LF") == 0)
            addItemTopList("%s", tmp.value);
        else
            parse_str(tmp.value);
        addItemTopList("\n");
    } else
        addItemTopList("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);
    if(tmp.alocated){
        free(tmp.value);
    }
    free(identific);

}

///------------LABEL GENERATORS------------------------

void label_if_else_end(char *func_name)
{
    addItemTopList("LABEL _%s_end_else_%d_%d\n",func_name, scope,pop_INT_Stack(&if_else_stack));
}

void label_for_end(char *func_name)
{
    addItemTopList("LABEL %s_for_end_%d_%d\n",func_name,scope,pop_INT_Stack(&for_stack));
}

///------------IF/ELSE FUNCTIONS------------------------

void if_cond(SyntaxNode *root,char *func_name)
{
    add_INT_StackItem(&if_else_stack, if_else_counter++);
    static int c = 0;
    addItemTopList("# DECLARE AND DEFAULT_INIT VAR %s%d\n","__IFCOND__", c);

    char temp1[15];
    sprintf(temp1, "__IFLEFT__%d", c);

    addItemTopList("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__IFRIGHT__%d", c++);
    addItemTopList("DEFVAR LF@%s\n", temp2);
    int countScopeLen = 1;
    int tmpScope = scope;
    while(tmpScope > 9){
        ++countScopeLen;
        tmpScope /= 10;
    }
    int countCLen = 1;
    int tmpC = c;
    while(tmpC > 9){
        ++countCLen;
        tmpC /= 10;
    }

    char *identific = (char*)malloc(sizeof(char)*strlen("__IFCOND__")+countCLen+1+countScopeLen+2);
    sprintf(identific, "%s%d_%d","__IFCOND__", c, scope);
    addItemTopList("DEFVAR LF@%s\n",identific);

    int type = getTypeOfExpr(root); //@TODO variable types !!!!
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(type));
    if(strcmp(tmp.type, "string") == 0){
        addItemTopList("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        if(!tmp.constant || strcmp(tmp.type, "LF") == 0)
            addItemTopList("%s", tmp.value);
        else
            parse_str(tmp.value);
        addItemTopList("\n");
    } else
        addItemTopList("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);

    if(tmp.alocated){
        free(tmp.value);
    }
    addItemTopList("JUMPIFNEQ _%s_else_%d_%d LF@%s bool@true\n",func_name,scope,top_INT_Stack(&if_else_stack),identific);
    free(identific);
}

void if_prefix(char *func_name)
{
    all_vars_to_new_scope();
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);

}

void if_suffix(char *func_name)
{
    addItemTopList("POPFRAME\n");
    deleteIdListScope(&currentScopeVars);
    all_vars_after_new_scope(scope);
    addItemTopList("JUMP _%s_end_else_%d_%d\n",func_name, scope,top_INT_Stack(&if_else_stack)); //end of if/else

}

void else_prefix(char *func_name)
{
    addItemTopList("LABEL _%s_else_%d_%d\n", func_name, scope,top_INT_Stack(&if_else_stack));
    all_vars_to_new_scope();
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
}

void else_suffix(char *func_name)
{

    addItemTopList("POPFRAME\n");
    deleteIdListScope(&currentScopeVars);
    all_vars_after_new_scope(scope);

}



///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------

//only after for and if/else scopes
void all_vars_to_new_scope()
{

    addItemTopList("# TRANSFER VARS TO NEW SCOPE\n");
    addItemTopList("CREATEFRAME\n");
    idList *currentScopeList = currentScopeVars;
    while(currentScopeList != NULL)
    {
        idListNode * currentScopeNode = currentScopeList->last;
        while (currentScopeNode != NULL) {

            addItemTopList("DEFVAR TF@%s_%d\n", currentScopeNode->name, currentScopeList->scope);
            addItemTopList("MOVE TF@%s_%d LF@%s_%d\n", currentScopeNode->name, currentScopeList->scope, currentScopeNode->name, currentScopeList->scope);
            currentScopeNode = currentScopeNode->prev;
        }
       // addItemTopList(">>>>>>>>>>>>>> %d\n", currentScopeList->scope);
        currentScopeList = currentScopeList->prevScope;

    }

}

//only after for and if/else scopes
void all_vars_after_new_scope(int scopeLessThan)
{
    addItemTopList("# TRANSFER VARS REAL VALUES\n");
    idList *currentScopeList = currentScopeVars;
    while(currentScopeList != NULL)
    {
        if(currentScopeList->scope > scopeLessThan){
            currentScopeList = currentScopeList->prevScope;
            continue;
        }
        idListNode * currentScopeNode = currentScopeList->last;
        while (currentScopeNode != NULL) {

            addItemTopList("MOVE LF@%s_%d TF@%s_%d\n", currentScopeNode->name, currentScopeList->scope, currentScopeNode->name, currentScopeList->scope);
            currentScopeNode = currentScopeNode->prev;
        }
        currentScopeList = currentScopeList->prevScope;

    }
    addItemTopList("# TRANSFER VARS's VALUES END\n");

}


///--------------------FOR FUNCTIONS-------------------------------

void for_header()
{
    all_vars_to_new_scope();
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    add_INT_StackItem(&for_stack, for_counter++);
}
void for_afterDeclaration(char* func_name){
    //for_loop_label
    all_vars_to_new_scope(scope);
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
}

//immediately after func for_header

void for_cond_to_loop(SyntaxNode *root,char *func_name)
{
    
    //section of init for_counter var
    static int c = 0;
    addItemTopList("# DECLARE AND DEFAULT_INIT VAR %s_%d\n","__FORCOND__", c);

    char temp1[15];
    sprintf(temp1, "__COLEFT__%d", c);
    addItemTopList("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__CORIGHT__%d", c++);
    addItemTopList("DEFVAR LF@%s\n", temp2);

    int countScopeLen = 1;
    int tmpScope = scope;
    while(tmpScope > 9){
        ++countScopeLen;
        tmpScope /= 10;
    }
    int countCLen = 1;
    int tmpC = c;
    while(tmpC > 9){
        ++countCLen;
        tmpC /= 10;
    }

    char *identific = (char*)malloc(sizeof(char)*strlen("__FORCOND__")+countCLen+1+countScopeLen+2);
    sprintf(identific, "%s%d_%d","__FORCOND__", c, scope);
    addItemTopList("DEFVAR LF@%s\n",identific);

    addItemTopList("LABEL %s_for_%d_%d\n",func_name,scope,top_INT_Stack(&for_stack));

    //TODO TYPE
    int type = getTypeOfExpr(root);
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(type));
    if(strcmp(tmp.type, "string") == 0){
        addItemTopList("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        if(!tmp.constant || strcmp(tmp.type, "LF") == 0)
            addItemTopList("%s", tmp.value);
        else
            parse_str(tmp.value);
        addItemTopList("\n");
    } else
        addItemTopList("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);

    //cond to loop
    addItemTopList("JUMPIFNEQ %s_for_end_%d_%d LF@%s bool@true\n",func_name,scope,top_INT_Stack(&for_stack),identific);
    free(identific);
}
void for_start_Assign(){
    deleteIdListScope(&currentScopeVars);
    addItemTopList("POPFRAME\n");
    all_vars_after_new_scope(scope);
    all_vars_to_new_scope(scope);
    addItemTopList("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
}

void for_suffix(char *func_name)
{
    //end for body

    deleteIdListScope(&currentScopeVars);
    addItemTopList("POPFRAME\n");
    all_vars_after_new_scope(scope);


    addItemTopList("JUMP %s_for_%d_%d\n",func_name,scope,top_INT_Stack(&for_stack));

    label_for_end(func_name);



    deleteIdListScope(&currentScopeVars);
    addItemTopList("POPFRAME\n");
    all_vars_after_new_scope(scope);


}
