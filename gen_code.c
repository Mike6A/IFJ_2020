/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 * @author Dávid Oravec (xorave06@stud.fit.vutbr.cz) implemented expressions
 */

#include "gen_code.h"


/*
///-----------INTERPRET STACK-----------------

void init_SN_Stack(tSN_Stack* list) {
    
    for(int i=0; i<MAX_STACK; ++i)
    {

        list->node[i] = NULL;

    }

    list->top = 0;

}

bool is_SN_Stack_Empty(tSN_Stack* stack) {

    return stack->top == 0;

}

int add_SN_StackItem(tSN_Stack* list, SyntaxNode* node) {

    list->node[++list->top] = node; 
    
}

SyntaxNode* Pop_SN_Stack(tSN_Stack* list) {

    
    return list->node[list->top--];
}

*/
static int scope = 0;
static int if_else_counter = 0;
static int for_counter = 0;
static bool skipDefvarAssignTemp = false;
static int assignC = 0;
idList * currentScopeVars = NULL;

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
            //fprintf(stderr, ">>>>>>>>>>>>> DELETE!!! %d <<<<<<<<<<<<<<<", (*list)->scope);
            deleteIdListScope(list);
        }
    }
}
void createNewIdListScope(idList** list){
    if((*list) != NULL)
        scope++;
    idList* newScopeList = malloc(sizeof(idList));
    if(newScopeList == NULL){
        return; //@TODO ERROR HANDLING!
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
            return; //@TODO ERROR HANDLING!
        }
        char * newName = malloc(sizeof(char) * strlen(name)+1);
        if(newName == NULL){
            free(newNode);
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
        while(currentScopeNode != NULL){ //@TODO strcmp(currentScopeNode->name, currentScope->first->name) != 0
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
    char *parsed_str[old_len];
    char *hex_arr[2];

    for(int i=1; i< old_len-1;i++)
    {

        ord = str[i];
        if((ord>=0 && ord<=32)||(ord == 35))
        {
            printf("\\%d%d",0,ord);
        }
        else if(ord == 92)
        {
                            
            switch(str[i+1])
            {
                                
                case 'n': 
                        printf("\\010");
                        break;

                case 't':
                        printf("\\009");
                        break;

                case '"':
                        printf("\\034");
                        break;

                case '\\':
                        printf("\\092");
                        break;
                //hex
                case 'x':
                        hex_arr[0] = str[i+2];
                        hex_arr[1] = str[i+3];
                        hex_arr[2] = '\0';
                        ord = hex_to_dec(&hex_arr);
                        printf("%c",ord);
                        i+=2;
                                            
            }
        ++i;
        }
        else
        {
            printf("%c",str[i]);
        }
                           
    }

}

///---------BUILT-IN FUNCTIONS-----------------

/// built-in string functions

void bif_lenght()
{
    printf("# func len(s string) (int)\n");
    printf("LABEL _len\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@len_ret_0_%d\n", scope);
    printf("MOVE LF@len_ret_0_%d int@0\n", scope);
    printf("STRLEN LF@len_ret_0_%d LF@arg_0\n", scope);
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_substr()
{
    printf("# func substr(s string,i int,n int) (string, int)\n");
    printf("LABEL _substr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@substr_ret_0_%d\n", scope);
    printf("MOVE LF@substr_ret_0_%d string@\n", scope);
    printf("DEFVAR LF@substr_ret_1_%d\n", scope);
    printf("MOVE LF@substr_ret_1_%d int@0\n", scope);
    printf("ADD LF@arg_1 LF@arg_1 int@1\n");
    printf("DEFVAR LF@in_string\n");
    printf("DEFVAR LF@str_len\n");
    printf("DEFVAR LF@substr_len\n");
    printf("STRLEN LF@str_len LF@arg_0\n");
    printf("MOVE LF@substr_len LF@str_len\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_1 int@0\n");
    printf("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    printf("GT LF@in_string LF@arg_1 LF@str_len\n");
    printf("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_2 int@0\n");
    printf("JUMPIFEQ _func_error_substr LF@in_string bool@true\n");
    printf("SUB LF@substr_len LF@substr_len LF@arg_1\n");
    printf("GT LF@in_string LF@arg_2 LF@substr_len\n");
    printf("JUMPIFNEQ _func_continue_substr LF@in_string bool@true\n");
    printf("ADD LF@substr_len LF@substr_len int@1\n");
    printf("MOVE LF@arg_2 LF@substr_len\n");
    printf("LABEL _func_continue_substr\n");
    printf("DEFVAR LF@index\n");
    printf("MOVE LF@index LF@arg_1\n");
    printf("SUB LF@index LF@index int@1\n");
    printf("DEFVAR LF@char_on_index\n");
    printf("DEFVAR LF@loop_counter\n");
    printf("MOVE LF@loop_counter int@0\n"); 
    printf("LABEL _func_loop_substr\n");
    printf("GETCHAR LF@char_on_index LF@arg_0 LF@index\n");
    printf("CONCAT LF@substr_ret_0_%d LF@substr_ret_0_%d LF@char_on_index\n", scope, scope);
    printf("ADD LF@index LF@index int@1\n");
    printf("ADD LF@loop_counter LF@loop_counter int@1\n");
    printf("JUMPIFNEQ _func_loop_substr LF@loop_counter LF@arg_2\n");
    printf("JUMP _func_end_substr\n");
    printf("LABEL _func_error_substr\n");
    printf("MOVE LF@substr_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_substr\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void bif_ord()
{
    printf("# func ord(s string,i int) (int, int)\n");
    printf("LABEL _ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ord_ret_0_%d\n", scope);
    printf("MOVE LF@ord_ret_0_%d int@0\n", scope);
    printf("DEFVAR LF@ord_ret_1_%d\n", scope);
    printf("MOVE LF@ord_ret_1_%d int@0\n", scope);
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_1 int@0\n");
    printf("JUMPIFEQ _func_error_ord LF@in_string bool@true\n");
    printf("DEFVAR LF@max_int_value\n");
    printf("STRLEN LF@max_int_value LF@arg_0\n");
    printf("SUB LF@max_int_value LF@max_int_value int@1\n");
    printf("GT LF@in_string LF@arg_1 LF@max_int_value\n");
    printf("JUMPIFEQ _func_error_ord LF@in_string bool@true\n" );
    printf("STRI2INT LF@ord_ret_0_%d LF@arg_0 LF@arg_1\n", scope );
    printf("JUMP _func_end_ord\n");
    printf("LABEL _func_error_ord\n");
    printf("MOVE LF@ord_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_ord\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_chr()
{
    printf("# func chr(i int) (string, int)\n");
    printf("LABEL _chr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@chr_ret_0_%d\n", scope);
    printf("MOVE LF@chr_ret_0_%d string@\n", scope);
    printf("DEFVAR LF@chr_ret_1_%d\n", scope);
    printf("MOVE LF@chr_ret_1_%d int@0\n", scope);
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_0 int@0\n");
    printf("JUMPIFEQ _func_error_chr LF@in_string bool@true\n");
    printf("GT LF@in_string LF@arg_0 int@255\n");
    printf("JUMPIFEQ _func_error_chr LF@in_string bool@true\n");
    printf("INT2CHAR LF@chr_ret_0_%d LF@arg_0\n", scope);
    printf("JUMP _func_end_chr\n");
    printf("LABEL _func_error_chr\n");
    printf("MOVE LF@chr_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_chr\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

///build-in functions for data type conversion

void bif_int2float()
{
    printf("# func int2float(i int) (float64 int)\n");
    printf("LABEL _int2float\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@int2float_ret_0_%d\n", scope);
    printf("MOVE LF@int2float_ret_0_%d float@0x1.2666666666666p+0\n",scope);
    printf("INT2FLOAT LF@int2float_ret_0_%d LF@arg_0\n",scope);
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void bif_float2int()
{
    printf("# func float2int(f float64) (int int)\n");
    printf("LABEL _float2int\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@float2int_ret_0_%d\n", scope);
    printf("MOVE LF@float2int_ret_0_%d int@0\n",scope);
    printf("FLOAT2INT LF@float2int_ret_0_%d LF@arg_0\n",scope);
    printf("POPFRAME\n");
    printf("RETURN\n");
}

///built-in function + parameters for value output

/*void bif_print_input(SyntaxNodes* my_statement)
{
    //@TODO CO TO SAKRA JE !!!
    int i=0;
    SyntaxNodes* current_statement = my_statement->first;

    //have to create before args pass(for non args func too)
    printf("CREATEFRAME\n");

        printf("# CREATE VARS FOR PRINT\n");

        while(current_statement->next != NULL)
        {

            printf("DEFVAR TF@arg_%d\n",i);
            printf("MOVE TF@arg_%d %s@%s\n",i, get_var_type(current_statement->node->token->type),
            current_statement->node->token->value);

            current_statement = current_statement->next;

        }

        printf("# ALL VARS FOR PRINT HAS BEEN CREATED\n");
      
}*/

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
        } 
    } 
      
    return dec_val; 
} 

void bif_print(SyntaxNodes* my_statement)
{

    int i=0;
    SyntaxNodes* current_statement = my_statement->first;
    static int c = 0;
    static int ord = 0;
    static char hex_arr[3];
    
    while(current_statement != NULL)
    {
        if(current_statement->node != NULL) {
            int whichScope = -1;
            switch (current_statement->node->type) {
                case Node_NumberIntExpression:
                    printf("WRITE int@%s\n", current_statement->node->right->token->value);
                    break;
                case Node_NumberDoubleExpression:
                    printf("WRITE float@%s\n", current_statement->node->right->token->value);
                    break;
                case Node_StringExpression:     
                    for(int i=1; i< strlen(current_statement->node->right->token->value)-1;i++)
                    {

                        ord = current_statement->node->right->token->value[i];
                        if((ord>=0 && ord<=32)||(ord == 35))
                        {
                            printf("WRITE string@\\%d%d\n",0,ord);
                        }
                            
                        else if(ord == 92)
                        {
                            
                            switch(current_statement->node->right->token->value[i+1])
                            {
                                
                                case 'n': 
                                        printf("WRITE string@\\010\n");
                                        break;

                                case 't':
                                        printf("WRITE string@\\009\n");
                                        break;

                                case '"':
                                        printf("WRITE string@\\034\n");
                                        break;
                                case '\\':
                                        printf("WRITE string@\\092\n");
                                        break;
                                //hex
                                case 'x':
                                        hex_arr[0] = current_statement->node->right->token->value[i+2];
                                        hex_arr[1] = current_statement->node->right->token->value[i+3];
                                        hex_arr[2] = '\0';
                                        ord = hex_to_dec(&hex_arr);
                                        printf("WRITE string@%c\n",ord);
                                        i+=2;
                                            
                            }
                            ++i;
                        }
                        else
                        {
                            printf("WRITE string@%c\n",current_statement->node->right->token->value[i]);
                        }
                           
                    }

                    /*if(strcmp(current_statement->node->right->token->value, "\n") == 0)
                        printf("WRITE string@\010\n");
                    else
                        printf("WRITE string@%s\n", current_statement->node->right->token->value);
                        */
                    break;
                case Node_IdentifierExpression:
                    whichScope = getLastScopeOfIdInList(currentScopeVars, current_statement->node->right->token->value);
                    printf("WRITE LF@%s_%d\n",current_statement->node->right->token->value , whichScope);
                    break;
                case Node_UnaryExpression:

                    printf("DEFVAR LF@%s\n", "tmpWriteExprUnary");
                    char temp1[15];
                    sprintf(temp1, "__WLEFT__%d", c);

                    printf("DEFVAR LF@%s\n", temp1);
                    char temp2[15];
                    sprintf(temp2, "__WRIGHT__%d", c++);
                    printf("DEFVAR LF@%s\n", temp2);

                    char *type = "int"; //@TODO
                    struct genExpr unary = GenParseExpr(current_statement->node, "tmpWriteExprUnary", temp1, temp2,
                                                        type);
                    printf("WRITE %s@%s%s\n", unary.constant ? unary.type : "LF" , unary.sign ? "-" : "", unary.value);
                    break;
                case Node_BinaryExpression:
                    printf("DEFVAR LF@%s\n", "tmpWriteExprBin");
                    char temp3[15];
                    sprintf(temp3, "__WLEFT__%d", c);

                    printf("DEFVAR LF@%s\n", temp3);
                    char temp4[15];
                    sprintf(temp4, "__WRIGHT__%d", c++);
                    printf("DEFVAR LF@%s\n", temp4);
                    char *typeBin = "int"; //@TODO
                    struct genExpr binary = GenParseExpr(current_statement->node, "tmpWriteExprBin", temp3, temp4, typeBin);
                    printf("WRITE %s@%s%s\n", binary.constant ? binary.type : "LF", binary.sign ? "-" : "", binary.value);
                    break;
            }
        }
        current_statement = current_statement->next;

    }
/*
    printf("POPFRAME\n");
    printf("RETURN\n");
    */
    
}

///built-in functions for read literates and print terms

void bif_inputs()
{
    printf("# func inputs() (string,int)\n");
    printf("LABEL _inputs\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputs_ret_0_%d\n", scope);
    printf("DEFVAR LF@inputs_ret_1_%d\n", scope);
    printf("MOVE LF@inputs_ret_1_%d int@0\n", scope);
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputs_ret_0_%d string\n", scope);
    printf("TYPE LF@input_type LF@inputs_ret__%d\n", scope);
    printf("JUMPIFNEQ _func_err_inputs LF@input_type string@string\n");
    printf("JUMP _func_end_inputs\n");
    printf("LABEL _func_err_inputs\n");
    printf("MOVE LF@inputs_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_inputs\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void bif_inputi()
{
    printf("# func inputi() (int,int)\n");
    printf("LABEL _inputi\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputi_ret_0_%d\n", scope);
    printf("DEFVAR LF@inputi_ret_1_%d\n", scope);
    printf("MOVE LF@inputi_ret_1_%d int@0\n", scope);
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputi_ret_0_%d int\n", scope);
    printf("TYPE LF@input_type LF@inputi_ret_0_%d\n", scope);
    printf("JUMPIFNEQ _func_err_inputi LF@input_type string@int\n");
    printf("JUMP _func_end_inputi\n");
    printf("LABEL _func_err_inputi\n");
    printf("MOVE LF@inputi_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_inputi\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

void bif_inputf()
{
    printf("# func inputf() (float64,int)\n");
    printf("LABEL _inputf\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputf_ret_0_%d\n", scope);
    printf("DEFVAR LF@inputf_ret_1_%d\n", scope);
    printf("MOVE LF@inputf_ret_1_%d int@0\n", scope);
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputf_ret_0_%d float\n", scope);
    printf("TYPE LF@input_type LF@inputf_ret_0_%d\n", scope);
    printf("JUMPIFNEQ _func_err_inputf LF@input_type string@float\n");
    printf("JUMP _func_end_inputf\n");
    printf("LABEL _func_err_inputf\n");
    printf("MOVE LF@inputf_ret_1_%d int@1\n", scope);
    printf("LABEL _func_end_inputf\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
}

///---------GENERATE PREFIX & SUFFIX FOR ALL FUNCTIONS-----------------

void program_start()
{

    printf("# START OF GEN_CODE\n");
    printf(".IFJcode20\n");
    printf("JUMP _main\n");

}

void main_prefix()
{

    printf("# MAIN FUNCTION\n");
    printf("LABEL _main\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);

}

void main_suffix()
{

    printf("# END OF MAIN FUNCTION\n");
    printf("POPFRAME\n");
    printf("CLEARS\n");
    printf("EXIT int@0\n");
    deleteIdListScope(&currentScopeVars);

}

void program_exit(tExpReturnType* ret_err) //TODO USING
{
    //deleteIdList(&currentScopeVars);
    //printf("EXIT int@%ld\n",ret_err.errCode);
    printf("# END OF GEN_CODE\n");

}

void general_func_call(char *func_name)
{
    //only after func_args_TF_declar
    //(only for func with argc>0)
    printf("CALL _%s\n",func_name);

}

void general_func_prefix(char *func_name, tFuncItem *func)
{

    printf("# START OF THE FUNCTION %s\n",func_name);
    printf("LABEL _%s\n",func_name);
    printf("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    for(int i = 0; i < func->params_count; ++i){
        addItemToIdList(&currentScopeVars, func->params[i]);
    }


}

void general_func_suffix(char *func_name)
{

    printf("LABEL _%s_ret\n", func_name);
    printf("POPFRAME\n");
    printf("RETURN\n");
    deleteIdListScope(&currentScopeVars);
    deleteIdListScope(&currentScopeVars);
}

void general_terminal_val(tToken token)
{

    char help_str[MAX_LEN];
    unsigned char c;

    switch(token.type)
    {

        case tokenType_INT:

            sprintf(help_str,"%s",token.value);
			printf("int@%s\n",help_str);
			break;

        case tokenType_DOUBLE:

            sprintf(help_str,"%s",token.value);
			printf("float@%s\n",help_str);
			break;

        case tokenType_STRING:

            for (int i = 0; (((c = (unsigned char) (token.value)[i]) != '\0') && (i < MAX_LEN)); i++)
			{
				help_str[i] = c;
			}

            printf("string@%s\n",help_str);
            break;

        case tokenType_ID:

            sprintf(help_str,"%s", token.value);
			printf("LF@%s\n",help_str);
			break;
        
        default:
            break;

    }

}

void declared_vars_default_init(TItem type)
{

    switch(type)
    {

        case TInt:
                printf("int@0\n");
                break;

        case TDouble: 
                printf("float@0.0\n");
                break;

        case TString:
                printf("string@\n");
                break;

        case TBool:
                printf("bool@false\n");
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

void no_built_in_func_args_TF_declar(char *func_name, tFuncItem *func, SyntaxNodes* paramValues)
{
    SyntaxNodes * currentParam = paramValues;
    //have to create before args pass(for non args func too)
    printf("CREATEFRAME\n");

        printf("# CREATE VARS FOR %s's ARGS\n",func_name);
        for(int i = 0; i < func->params_count ; i++)
        {
            char paramId[64];
            sprintf(paramId, "%s__P__%d",func_name, i);

            printf("DEFVAR LF@%s\n",paramId);
            char temp1[64];
            sprintf(temp1, "%s__LEFT__%d",func_name, i);

            printf("DEFVAR LF@%s\n", temp1);
            char temp2[64];
            sprintf(temp2, "%s__RIGHT__%d",func_name, i);
            printf("DEFVAR LF@%s\n", temp2);
            struct genExpr param = GenParseExpr(currentParam->node, paramId, temp1, temp2, get_var_type(func->paramsTypes[i]));
            printf("DEFVAR TF@%s\n",func->params[i]);
            printf("MOVE TF@%s %s@%s%s\n",func->params[i], param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            //declared_vars_default_init(func->paramsTypes[i]);
            currentParam = currentParam->next;

        }

        printf("# ALL VARS FOR ARGS HAS BEEN CREATED\n");
      
}

void func_args_TF_declar(char *func_name, tFuncItem *func, SyntaxNodes* paramValues)
{
    static int called = 0;
    SyntaxNodes * currentParam = paramValues;
    //have to create before args pass(for non args func too)
    printf("CREATEFRAME\n");

        printf("# CREATE VARS FOR %s's ARGS\n",func_name);
        for(int i = 0; i < func->params_count ; i++)
        {
            char paramId[64];
            sprintf(paramId, "%s__P__%d_%d",func_name, i,called);

            printf("DEFVAR LF@%s\n",paramId);
            char temp1[64];
            sprintf(temp1, "%s__LEFT__%d_%d",func_name, i,called);

            printf("DEFVAR LF@%s\n", temp1);
            char temp2[64];
            sprintf(temp2, "%s__RIGHT__%d_%d",func_name, i,called++);
            printf("DEFVAR LF@%s\n", temp2);
            struct genExpr param = GenParseExpr(currentParam->node, paramId, temp1, temp2, get_var_type(func->paramsTypes[i]));
            if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                printf("DEFVAR TF@%s\n",func->params[i]);
            } else{
                printf("DEFVAR TF@%s_%d\n",func->params[i],0);
            }
            if(strcmp(param.type, "string") == 0){
                if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                    printf("MOVE TF@%s %s@%s",func->params[i], param.constant? param.type : "LF", param.sign ? "-":"");
                }else
                    printf("MOVE TF@%s_%d %s@%s",func->params[i], 0, param.constant? param.type : "LF", param.sign ? "-":"");
                parse_str(param.value);
                printf("\n");
            } else{
            if(strcmp(func->params[i], "arg_0") == 0 || strcmp(func->params[i], "arg_1") == 0 || strcmp(func->params[i], "arg_2") == 0){
                printf("MOVE TF@%s %s@%s%s\n",func->params[i], param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            }else
                printf("MOVE TF@%s_%d %s@%s%s\n",func->params[i], 0, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            }
            //declared_vars_default_init(func->paramsTypes[i]);
            currentParam = currentParam->next;
            if(param.alocated)
                free(param.value);
        }

        printf("# ALL VARS FOR ARGS HAS BEEN CREATED\n");
      
}

void func_ret_to_LF(char *func_name, tFuncItem *func, SyntaxNodes* dest)
{
    SyntaxNodes* currentDest = dest;
    //printf(">>>>>>>>>>>>>%s\n",currentDest->node->name);
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
            int whichScopeRet = getLastScopeOfIdInList(currentScopeVars, retId);
            printf("MOVE LF@%s_%d TF@%s_%d\n",currentDest->node->left->token->value,whichScope, retId);
            free(retId);
        }
        currentDest = currentDest->next;
    }

}

//current var have to be declared before func
//use only after selected func
void var_assign_to_func_value(char *func_name, tHashItem *item)
{

    printf("MOVE LF@%s TF@%s_ret_0\n",item->id, func_name);

}

void func_ret_declar(char *func_name, tFuncItem *func)
{

    printf("# DECLA VALUES FOR %s's RETURNS\n",func_name);
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
        printf("DEFVAR LF@%s_%d\n",retId, whichScope);
        printf("MOVE LF@%s_%d ",retId,scope);
        free(retId);
        declared_vars_default_init(func->return_vals[i]);

    }
    createNewIdListScope(&currentScopeVars);
    printf("# ALL VARS FOR RETURNS HAS BEEN DECLARED\n");
    
}

void func_ret_get_value(char *func_name, tFuncItem *func, SyntaxNodes* retValues)
{

    SyntaxNodes * currentRet = retValues;
    //only after func_ret_declar

        printf("# MOVE VALUES FOR %s's RETURNS\n",func_name);
        for(int i = 0; i<func->return_count; i++)
        {
            
            char retId[64];
            sprintf(retId, "%s__P__%d",func_name, i);

            printf("DEFVAR LF@%s\n",retId);
            char temp1[64];
            sprintf(temp1, "%s__LEFT__%d",func_name, i);

            printf("DEFVAR LF@%s\n", temp1);
            char temp2[64];
            sprintf(temp2, "%s__RIGHT__%d",func_name, i);
            printf("DEFVAR LF@%s\n", temp2);
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
                printf("MOVE LF@%s_%d %s@%s",retValueId,whichRetValueScope, param.constant? param.type : "LF", param.sign ? "-":"");
                parse_str(param.value);
                printf("\n");
            } else
                printf("MOVE LF@%s_%d %s@%s%s\n",retValueId,whichRetValueScope, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            //printf("MOVE LF@%s_ret_%d %s@%s%s\n",func_name,i, param.constant? param.type : "LF", param.sign ? "-":"", param.value );
            free(retValueId);
            currentRet = currentRet->next;
            if(param.alocated)
                free(param.value);
        }
        for(int x = scope-1; x > 0; --x){
            printf("POPFRAME\n");
            all_vars_after_new_scope(x);
        }
        printf("JUMP _%s_ret\n", func_name);
        printf("# ALL VARS FOR RETURNS HAS BEEN INITTED\n");
    
}

///----------------FUNC FOR VARIABLES NAD EXPR-------------
/*
void stack_most_right(tSN_Stack *stack, SyntaxNode *root)
{

    for(SyntaxNode *tmp = root; (tmp!=NULL)
    &&(tmp->type==Node_BinaryExpression || 
       tmp->type==Node_UnaryExpression); tmp = tmp->right)
    {

        add_SN_StackItem(stack, tmp);
       // printf("%s\n",tmp->name);
    }
        
}

void vars_final_counter(SyntaxNode *root, tHashItem *item)
{
    tSN_Stack stack;

    init_SN_Stack(&stack);
    stack_most_right(&stack,root);
    
    printf("DEFVAR LF@tmp\n");
    printf("%s\n",stack.node[stack.top]->right->right->name);
    printf("MOVE LF@tmp %s@%s\n",get_var_type(item->type),stack.node[stack.top]->right->right->token->value);
   // printf("MOVE LF@\%tmp ",stack->node->right->token->value);
    while(!is_SN_Stack_Empty(&stack))
    {

        SyntaxNode *node = Pop_SN_Stack(&stack);
        stack_most_right(&stack,node->left);

    }

}

*/


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
            char *identifier = malloc(sizeof(char)*strlen(root->right->token->value)+1+countScopeLen+2); // TODO WTF ? PRECO 2
            test.alocated = true;
            sprintf(identifier, "%s_%d", root->right->token->value, whichScope);
            test.value = identifier;
        }else {
            test.alocated = false;
            test.value = root->right->token->value;
        }
        test.type = root->type == Node_IdentifierExpression ? "LF" : type;

        if(strcmp(test.type, "float") == 0){
            double f = atof(test.value);

            size_t needed = snprintf(NULL, 0,"%a", f)+1;
            //fprintf(stderr, ">>>>>>>>>>>>>>>>>>>> %ld", needed);
            test.value = malloc(needed);

            sprintf(test.value, "%a", f); // @TODO DANGER!!!!!!
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
                    printf("CONCAT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    parse_str(leftTemp.value);
                    printf(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    parse_str(rightTemp.value);
                    printf("\n");
                }else {
                    printf("ADD LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            case tokenType_MINUS:
                printf("SUB LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_MUL:
                printf("MUL LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_DIV:
                //printf("\n"); // IF rightTemp.value != 0 then jump on LABEL
                if((strcmp(rightTemp.value,"0") == 0)||(strcmp(rightTemp.value,"0.0") == 0))
                {
                    printf("DPRINT %s", "divide by zero\n");
                    printf("EXIT int@9\n");
                }
                if (strcmp(type, "int") == 0) {
                    printf("IDIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                } else if (strcmp(type, "float") == 0) {
                    printf("DIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type: "LF", leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
        }
        if(changeSign){
            if (strcmp(type, "int") == 0) {
                printf("MUL LF@%s %s@%s %s@%s\n", assignTo, "LF", assignTo, "int", "-1");
            }else if (strcmp(type, "float") == 0) {
                printf("MUL LF@%s %s@%s %s@%s\n", assignTo, "LF", assignTo, "float", "-1.0");
            }
            changeSign = false;
        }
        if(leftTemp.alocated){
            free(leftTemp.value); //@TODO VALGRIND CHECK
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
                    printf("EQ LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    parse_str(leftTemp.value);
                    printf(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    parse_str(rightTemp.value);
                    printf("\n");
                }else {
                    printf("EQ LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                    }
                break;
            case tokenType_GE:
                //@TODO
                break;
            case tokenType_LE:
                //@TODO
                break;
            case tokenType_NEQ:
                //@TODO
                break;
            case tokenType_GREATER:
                if(strcmp(type, "string") == 0){
                    printf("GT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    parse_str(leftTemp.value);
                    printf(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    parse_str(rightTemp.value);
                    printf("\n");
                }else {
                    printf("GT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            case tokenType_LESS:
                if(strcmp(type, "string") == 0){
                    printf("LT LF@%s %s@%s", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "");
                    parse_str(leftTemp.value);
                    printf(" %s@%s", rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "");
                    parse_str(rightTemp.value);
                    printf("\n");
                }else {
                    printf("LT LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.constant ? leftTemp.type : "LF",
                           leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.constant ? rightTemp.type : "LF", rightTemp.sign ? "-" : "", rightTemp.value);
                }
                break;
            default:
                break;
        }
        if(leftTemp.alocated){
            free(leftTemp.value); //@TODO VALGRIND CHECK
            leftTemp.alocated = false;
        }
        if(rightTemp.alocated){
            free(rightTemp.value);
            rightTemp.alocated = false;
        }
        struct genExpr done = {.value = assignTo, .type = type, .sign = false};
        return done;
    }
}

void GENASIGN(SyntaxNode* root, tHashItem* item){ //TEST!!!!!!!!
    static int c = 0;
    printf("DEFVAR %s\n", item->id);
    char temp1[15];
    sprintf(temp1, "__LEFT__%d", c);

    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__RIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);

    struct genExpr tmp = GenParseExpr(root, item->id, temp1, temp2, get_var_type(item->type));
    printf("MOVE LF@%s LF@%s\n", item->id, tmp.value);
}

void vars_default_declar_init(SyntaxNode *root, tHashItem *item)
{
    //@TODO MALLOC
    printf("# DECLARE AND DEFAULT_INIT VAR %s\n",item->id);
    static int c = 0;
    char temp1[15];
    sprintf(temp1, "__DLEFT__%d", c);

    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__DRIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);
    int countScopeLen = 1;
    int tmpscope = scope;
    while(tmpscope > 9){
        ++countScopeLen;
        tmpscope /= 10;
    }
    char *identific = malloc(sizeof(char)*strlen(item->id)+1+countScopeLen+2 ); // TODO WTF??? PRECO 2
    sprintf(identific, "%s_%d", item->id, scope);
    addItemToIdList(&currentScopeVars, item->id);
    printf("DEFVAR LF@%s\n",identific);
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(item->type));
    if(strcmp(tmp.type, "string") == 0){
        printf("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        parse_str(tmp.value);
        printf("\n");
    } else
        printf("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);
    if(tmp.alocated){
        free(tmp.value);
    }
    free(identific);
    //GenParseExpr(root,assignTo,right,left));

}

// only use after vars_default_declar_init!!
void vars_set_new_value(SyntaxNode *root, tHashItem *item)
{
    // @TODO MALLOC
    char temp1[15];
    sprintf(temp1, "__ALEFT__%d", assignC);
    char temp2[15];
    sprintf(temp2, "__ARIGHT__%d", assignC++);
    if(!skipDefvarAssignTemp){
        printf("DEFVAR LF@%s\n", temp1);
        printf("DEFVAR LF@%s\n", temp2);
    }


    //printf("DEFVAR LF@%s\n",item->id);
    int countScopeLen = 1;
    int whichScope = getLastScopeOfIdInList(currentScopeVars, item->id);
    int tmpScope = whichScope;
    while(tmpScope > 9){
        ++countScopeLen;
        tmpScope /= 10;
    }
    char *identific = malloc(sizeof(char)*strlen(item->id)+1+countScopeLen+1); // TODO MALLOC
    sprintf(identific, "%s_%d", item->id, tmpScope);
    printf("# RE-SET VALUE FOR VAR %s\n",item->id);
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(item->type));
    if(strcmp(tmp.type, "string") == 0){
        printf("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        parse_str(tmp.value);
        printf("\n");
    } else
        printf("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);
    //GenParseExpr(root,assignTo,right,left));
    if(tmp.alocated){
        free(tmp.value);
    }
    free(identific);

}

///------------LABEL GENERATORS------------------------

void label_if_else_end(char *func_name)
{
    printf("LABEL _%s_end_else_%d_%d\n",func_name, scope,if_else_counter);
}

void label_for_end(char *func_name)
{
    printf("LABEL %s_for_end_%d_%d\n",func_name,scope,for_counter);
}

///------------IF/ELSE FUNCTIONS------------------------

void if_cond(SyntaxNode *root,char *func_name)
{
    static int c = 0;
    printf("# DECLARE AND DEFAULT_INIT VAR %s%d\n","__IFCOND__", c);

    char temp1[15];
    sprintf(temp1, "__IFLEFT__%d", c);

    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__IFRIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);
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

    char *identific = (char*)malloc(sizeof(char)*strlen("__IFCOND__")+countCLen+1+countScopeLen+2); // TODO WTF ? PRECO 2
    sprintf(identific, "%s%d_%d","__IFCOND__", c, scope);
    printf("DEFVAR LF@%s\n",identific);

    int type = TInt; //@TODO variable types !!!!
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(type));
    if(strcmp(tmp.type, "string") == 0){
        printf("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        parse_str(tmp.value);
        printf("\n");
    } else
        printf("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);

    if(tmp.alocated){
        free(tmp.value);
    }
    ++if_else_counter;
    printf("JUMPIFNEQ _%s_else_%d_%d LF@%s bool@true\n",func_name,scope,if_else_counter,identific);
    free(identific);
}

void if_prefix(char *func_name)
{
    all_vars_to_new_scope(); //@TODO TO SOLVE
    printf("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    //scope++;
    //begin if body

}

void if_suffix(char *func_name)
{
    //end if body
    printf("POPFRAME\n");
    deleteIdListScope(&currentScopeVars);
    all_vars_after_new_scope(scope); // @TODO TO solve
    printf("JUMP _%s_end_else_%d_%d\n",func_name, scope,if_else_counter); //end of if/else

}

void else_prefix(char *func_name)
{
    printf("LABEL _%s_else_%d_%d\n", func_name, scope,if_else_counter);
    all_vars_to_new_scope();//@TODO
    printf("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    //scope++;
}

void else_suffix(char *func_name)
{

    printf("POPFRAME\n");
    deleteIdListScope(&currentScopeVars);
    all_vars_after_new_scope(scope); //@TODO

}



///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------

//only after for and if/else scopes
void all_vars_to_new_scope()
{

    printf("# TRANSFER VARS TO NEW SCOPE\n");
    printf("CREATEFRAME\n");
    idList *currentScopeList = currentScopeVars;
    while(currentScopeList != NULL)
    {
        idListNode * currentScopeNode = currentScopeList->last;
        while (currentScopeNode != NULL) {

            printf("DEFVAR TF@%s_%d\n", currentScopeNode->name, currentScopeList->scope);
            printf("MOVE TF@%s_%d LF@%s_%d\n", currentScopeNode->name, currentScopeList->scope, currentScopeNode->name, currentScopeList->scope);
            currentScopeNode = currentScopeNode->prev;
        }
       // printf(">>>>>>>>>>>>>> %d\n", currentScopeList->scope);
        currentScopeList = currentScopeList->prevScope;

    }

}

//only after for and if/else scopes
void all_vars_after_new_scope(int scopeLessThan)
{
    printf("# TRANSFER VARS REAL VALUES\n");
    idList *currentScopeList = currentScopeVars;
    while(currentScopeList != NULL)
    {
        if(currentScopeList->scope > scopeLessThan){
            currentScopeList = currentScopeList->prevScope;
            continue;
        }
        idListNode * currentScopeNode = currentScopeList->last;
        while (currentScopeNode != NULL) {

            printf("MOVE LF@%s_%d TF@%s_%d\n", currentScopeNode->name, currentScopeList->scope, currentScopeNode->name, currentScopeList->scope);
            currentScopeNode = currentScopeNode->prev;
        }
        currentScopeList = currentScopeList->prevScope;

    }
    printf("# TRANSFER VARS's VALUES END\n");

}


///--------------------FOR FUNCTIONS-------------------------------

void for_header()
{

    all_vars_to_new_scope();
    printf("PUSHFRAME\n");
    createNewIdListScope(&currentScopeVars);
    //section of init for_counter var
//    printf("# DECLARE AND DEFAULT_INIT VAR %s\n",item->id);
//    static int c = 0;
//    char temp1[15];
//    sprintf(temp1, "__LEFT__%d", c);
//
//    printf("DEFVAR LF@%s\n", temp1);
//    char temp2[15];
//    sprintf(temp2, "__RIGHT__%d", c++);
//    printf("DEFVAR LF@%s\n", temp2);
//    char identific[64]; // TODO MALLOC
//    sprintf(identific, "%s_%d", item->id, scope);
//    printf("DEFVAR LF@%s\n",identific);
//    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(item->type));
//    if(strcmp(tmp.type, "string") == 0){
//        printf("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
//        parse_str(tmp.value);
//        printf("\n");
//    } else
//        printf("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);
    
}
void for_afterDeclaration(char* func_name){
    //for_loop_label
    all_vars_to_new_scope(scope);
    printf("PUSHFRAME\n");
    //TODO MALLOC
    char temp1[15];
    sprintf(temp1, "__ALEFT__%d", assignC);
    char temp2[15];
    sprintf(temp2, "__ARIGHT__%d", assignC);
    printf("DEFVAR LF@%s\n", temp1);
    printf("DEFVAR LF@%s\n", temp2);

}

//immediately after func for_header

void for_cond_to_loop(SyntaxNode *root,char *func_name)
{
    
    //section of init for_counter var
    static int c = 0;
    //all_vars_to_new_scope();
    createNewIdListScope(&currentScopeVars);
    printf("# DECLARE AND DEFAULT_INIT VAR %s_%d\n","__FORCOND__", c);

    char temp1[15];
    sprintf(temp1, "__COLEFT__%d", c);
    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__CORIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);

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

    char *identific = (char*)malloc(sizeof(char)*strlen("__FORCOND__")+countCLen+1+countScopeLen+2); // TODO WTF ? PRECO 2
    sprintf(identific, "%s%d_%d","__FORCOND__", c, scope);
    printf("DEFVAR LF@%s\n",identific);

    printf("LABEL %s_for_%d_%d\n",func_name,scope,for_counter);

    //TODO TYPE
    int type = TInt;
    struct genExpr tmp = GenParseExpr(root, identific, temp1, temp2, get_var_type(type));
    if(strcmp(tmp.type, "string") == 0){
        printf("MOVE LF@%s %s@%s",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"");
        parse_str(tmp.value);
        printf("\n");
    } else
        printf("MOVE LF@%s %s@%s%s\n",identific, tmp.constant ? tmp.type: "LF",tmp.sign?"-":"", tmp.value);

    //cond to loop
    printf("JUMPIFNEQ %s_for_end_%d_%d LF@%s bool@true\n",func_name,scope,for_counter,identific);

}
void for_start_Assign(){
    skipDefvarAssignTemp = true;
};
void for_end_Assign(){
    skipDefvarAssignTemp = false;
}
void for_prefix(char *func_name)
{
    //all_vars_to_new_scope();
    printf("PUSHFRAME\n");
    scope++;
    //begin for body

}

void for_suffix(char *func_name)
{
    //end for body

    printf("JUMP %s_for_%d_%d\n",func_name,scope,for_counter);
    label_for_end(func_name);
    deleteIdListScope(&currentScopeVars);
    printf("POPFRAME\n");

    all_vars_after_new_scope(scope);

    deleteIdListScope(&currentScopeVars);
    printf("POPFRAME\n");
    all_vars_after_new_scope(scope);

    //all_vars_after_new_scope();
    //assignment

}
