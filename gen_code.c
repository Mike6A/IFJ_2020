/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
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

///---------BUILT-IN FUNCTIONS-----------------

/// built-in string functions

void bif_lenght()
{

    printf("# func len(s string) (int)\n");
    printf("LABEL _len\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@len_ret_0\n");
    printf("MOVE LF@len_ret_0 int@0\n");
    printf("STRLEN LF@len_ret_0 LF@arg_0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_substr()
{

    printf("# func substr(s string,i int,n int) (string, int)\n");
    printf("LABEL _substr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@substr_ret_0\n");
    printf("MOVE LF@substr_ret_0 string@\n");
    printf("DEFVAR LF@substr_ret_1\n");
    printf("MOVE LF@substr_ret_1 int@0\n");
    printf("DEFVAR LF@in_string\n");
    printf("DEFVAR LF@str_len\n");
    printf("DEFVAR LF@substr_len\n");
    printf("STRLEN LF@str_len LF@arg_0\n");
    printf("MOVE LF@substr_len LF@str_len\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_1 int@1\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("GT LF@in_string LF@arg_1 LF@str_len\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_2 int@0\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("SUB LF@substr_len LF@substr_len LF@arg_1\n");
    printf("GT LF@in_string LF@arg_2 LF@substr_len\n");
    printf("JUMPIFNEQ _func_continue LF@in_string bool@true\n");
    printf("ADD LF@substr_len LF@substr_len int@1\n");
    printf("MOVE LF@arg_2 LF@substr_len\n");
    printf("LABEL _func_continue\n");
    printf("DEFVAR LF@index\n");
    printf("MOVE LF@index LF@arg_1\n");
    printf("SUB LF@index LF@index int@1\n");
    printf("DEFVAR LF@char_on_index\n");
    printf("DEFVAR LF@loop_counter\n");
    printf("MOVE LF@loop_counter int@0\n"); 
    printf("LABEL _func_loop\n");
    printf("GETCHAR LF@char_on_index LF@arg_0 LF@index\n");
    printf("CONCAT LF@substr_ret_0 LF@substr_ret_0 LF@char_on_index\n");
    printf("ADD LF@index LF@index int@1\n");
    printf("ADD LF@loop_counter LF@loop_counter int@1\n");
    printf("JUMPIFNEQ _func_loop LF@loop_counter LF@arg_2\n");
    printf("JUMP _func_end\n");
    printf("LABEL _func_error\n");
    printf("MOVE LF@substr_ret_1 int@1\n");
    printf("LABEL _func_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_ord()
{

    printf("# func ord(s string,i int) (int, int)\n");
    printf("LABEL _ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@ord_ret_0\n");
    printf("MOVE LF@ord_ret_0 int@0\n");
    printf("DEFVAR LF@ord_ret_1\n");
    printf("MOVE LF@ord_ret_1 int@0\n");
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_1 int@0\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("DEFVAR LF@max_int_value\n");
    printf("STRLEN LF@max_int_value LF@arg_0\n");
    printf("SUB LF@max_int_value LF@max_int_value int@1\n");
    printf("GT LF@in_string LF@arg_1 LF@max_int_value\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n" );
    printf("STRI2INT LF@ord_ret_0 LF@arg_0 LF@arg_1\n" );
    printf("JUMP _func_end\n");
    printf("LABEL _func_error\n");
    printf("MOVE LF@ord_ret_1 int@1\n");
    printf("LABEL _func_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_chr()
{

    printf("# func chr(i int) (string, int)\n");
    printf("LABEL _chr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@chr_ret_0\n");
    printf("MOVE LF@chr_ret_0 string@\n");
    printf("DEFVAR LF@chr_ret_1\n");
    printf("MOVE LF@chr_ret_1 int@0\n");
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_0 int@0\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("GT LF@in_string LF@arg_0 int@255\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("INT2CHAR LF@chr_ret_0 LF@arg_0\n");
    printf("JUMP _func_end\n");
    printf("LABEL _func_error\n");
    printf("MOVE LF@chr_ret_1 int@1\n");
    printf("LABEL _func_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

///build-in functions for data type conversion

void bif_int2float()
{

    printf("# func int2float(i int) (float64 int)\n");
    printf("LABEL _int2float\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@int2float_ret_0\n");
    printf("MOVE LF@int2float_ret_0 float@0x1.2666666666666p+0\n");
    printf("INT2FLOAT LF@int2float_ret_0 LF@arg_0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_float2int()
{

    printf("# func float2int(f float64) (int int)\n");
    printf("LABEL _float2int\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@float2int_ret_0\n");
    printf("MOVE LF@int2float_ret_0 int@0\n");
    printf("FLOAT2INT LF@float2int_ret_0 LF@arg_0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

///built-in function + parameters for value output

void bif_print_input(SyntaxNodes* my_statement)
{

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
      
}

void bif_print(SyntaxNodes* my_statement)
{

    int i=0;
    SyntaxNodes* current_statement = my_statement->first;

    printf("# func print (term1,term2,...,term𝑛)\n");
    printf("LABEL _print\n");
    printf("PUSHFRAME\n");

    while(current_statement->next != NULL)
    {
        if(strcmp(current_statement->node->token->value,"\n") == 0)
            printf("WRITE string@\010\n");
        else
            printf("WRITE LF@arg_%d\n",i);


        current_statement = current_statement->next;

    }    

    printf("POPFRAME\n");
    printf("RETURN\n");
    
}

///built-in functions for read literates and print terms

void bif_inputs()
{

    printf("# func inputs() (string,int)\n");
    printf("LABEL _inputs\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputs_ret_0\n");
    printf("DEFVAR LF@inputs_ret_1\n");
    printf("MOVE LF@inputs_ret_1 int@0\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputs_ret_0 string\n");
    printf("TYPE LF@input_type LF@inputs_ret_0\n");
    printf("JUMPIFNEQ _func_err LF@input_type string@string\n");
    printf("JUMP _func_end\n");
    printf("LABEL _func_err\n");
    printf("MOVE LF@inputs_ret_1 int@1\n");
    printf("LABEL _func_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_inputi()
{

    printf("# func inputi() (int,int)\n");
    printf("LABEL _inputi\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputi_ret_0\n");
    printf("DEFVAR LF@inputi_ret_1\n");
    printf("MOVE LF@inputi_ret_1 int@0\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputi_ret_0 int\n");
    printf("TYPE LF@input_type LF@inputi_ret_0\n");
    printf("JUMPIFNEQ _func_err LF@input_type string@int\n");
    printf("JUMP _func_end\n");
    printf("LABEL _func_err\n");
    printf("MOVE LF@inputi_ret_1 int@1\n");
    printf("LABEL _func_end\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_inputf()
{

    printf("# func inputf() (float64,int)\n");
    printf("LABEL _inputf\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@inputf_ret_0\n");
    printf("DEFVAR LF@inputf_ret_1\n");
    printf("MOVE LF@inputf_ret_1 int@0\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@inputf_ret_0 float\n");
    printf("TYPE LF@input_type LF@inputf_ret_0\n");
    printf("JUMPIFNEQ _func_err LF@input_type string@float\n");
    printf("JUMP _func_end\n");
    printf("LABEL _func_err\n");
    printf("MOVE LF@inputf_ret_1 int@1\n");
    printf("LABEL _func_end\n");
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

}

void main_suffix()
{

    printf("# END OF MAIN FUNCTION\n");
    printf("POPFRAME\n");
    printf("CLEARS\n");

}

void program_exit(tExpReturnType ret_err) //TODO USING
{

    printf("EXIT int@%ld\n",ret_err.errCode);
    printf("# END OF GEN_CODE\n");

}

void general_func_call(char *func_name)
{
    //only after func_args_TF_declar
    //(only for func with argc>0)
    printf("CALL _%s\n",func_name);

}

void general_func_prefix(char *func_name)
{

    printf("# START OF THE FUNCTION %s\n",func_name);
    printf("LABEL _%s\n",func_name);
    printf("PUSHFRAME\n");

}

void general_func_suffix(char *func_name)
{

    printf("LABEL _%s_ret\n", func_name);
    printf("POPFRAME\n");
    printf("RETURN\n");

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

        case TDouble: //ASK: why not TFloat
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

void func_args_TF_declar(char *func_name, tFuncItem *func)
{

    //have to create before args pass(for non args func too)
    printf("CREATEFRAME\n");

        printf("# CREATE VARS FOR %s's ARGS\n",func_name);
        for(int i = 0; i < func->params_count ; i++)
        {
            
            printf("DEFVAR TF@arg_%d\n",i);
            printf("MOVE TF@arg_%d ",i);
            declared_vars_default_init(func->paramsTypes[i]);

        }

        printf("# ALL VARS FOR ARGS HAS BEEN CREATED\n");
      
}

void func_ret_declar(char *func_name, tFuncItem *func)
{

    printf("# DECLA VALUES FOR %s's RETURNS\n",func_name);
    for(int i = 0; i<func->return_count; i++)
    {
            
        printf("DEFVAR LF@%s_ret_%d\n",func_name, i);
        printf("MOVE LF@%s_ret_%d ",func_name,i);
        declared_vars_default_init(func->return_vals[i]);

    }

    printf("# ALL VARS FOR RETURNS HAS BEEN DECLARED\n");
    
}

void func_ret_get_value(char *func_name, tFuncItem *func)
{

    //only after func_ret_declar

        printf("# MOVE VALUES FOR %s's RETURNS\n",func_name);
        for(int i = 0; i<func->return_count; i++)
        {
            
            printf("MOVE LF@%s_ret_%d ",func_name,i);
            declared_vars_default_init(func->return_vals[i]);

        }

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
    struct genExpr rightTemp = {.value = right, .type = type, .sign = false};
    struct genExpr leftTemp = {.value = left, .type = type, .sign = false};

    if(root->type == Node_NumberIntExpression || root->type == Node_NumberDoubleExpression || root->type == Node_StringExpression || root->type == Node_IdentifierExpression) {

        struct genExpr test;
        test.value = root->right->token->value;
        test.type = root->type == Node_IdentifierExpression ? "LF" : type;
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
                printf("ADD LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.type, leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.type, rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_MINUS:
                printf("SUB LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.type, leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.type, rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_MUL:
                printf("MUL LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.type, leftTemp.sign ? "-" : "", leftTemp.value,
                       rightTemp.type, rightTemp.sign ? "-" : "", rightTemp.value);
                break;
            case tokenType_DIV:
                //ASK: EXITNUT ABO CO DO PICE
                printf("\n"); // IF rightTemp.value != 0 then jump on LABEL
                printf("DPRINT %s", "divide by zero\n");
                printf("EXIT int@5\n");
                printf("\n"); // LABEL IF
                if (strcmp(type, "int") == 0) {
                    printf("IDIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.type, leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.type, rightTemp.sign ? "-" : "", rightTemp.value);
                } else if (strcmp(type, "float") == 0) {
                    printf("DIV LF@%s %s@%s%s %s@%s%s\n", assignTo, leftTemp.type, leftTemp.sign ? "-" : "", leftTemp.value,
                           rightTemp.type, rightTemp.sign ? "-" : "", rightTemp.value);
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

    printf("# DECLARE AND DEFAULT_INIT VAR %s\n",item->id);
    static int c = 0;
    char temp1[15];
    sprintf(temp1, "__LEFT__%d", c);

    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__RIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);

    printf("DEFVAR LF@%s\n",item->id);
    struct genExpr tmp = GenParseExpr(root, item->id, temp1, temp2, get_var_type(item->type));
    printf("MOVE LF@%s %s@%s%s\n",item->id,tmp.type,tmp.sign?"-":"", tmp.value);
    //GenParseExpr(root,assignTo,right,left));

}

// only use after vars_default_declar_init!!
void vars_set_new_value(SyntaxNode *root, tHashItem *item)
{
    //@TODO
    static int c = 0;
    char temp1[15];
    sprintf(temp1, "__LEFT__%d", c);

    printf("DEFVAR LF@%s\n", temp1);
    char temp2[15];
    sprintf(temp2, "__RIGHT__%d", c++);
    printf("DEFVAR LF@%s\n", temp2);

    printf("DEFVAR LF@%s\n",item->id);


    printf("# RE-SET VALUE FOR VAR %s\n",item->id);
    struct genExpr tmp = GenParseExpr(root, item->id, temp1, temp2, get_var_type(item->type));
    printf("MOVE LF@%s %s@%s%s\n",item->id,tmp.type, tmp.sign?"-":"", tmp.value);
    //GenParseExpr(root,assignTo,right,left));

}

///---------STACK FUNCTIONS-----------------

void stack_concat_string(char *str3, char *str2, char *str1)
{
    
    printf("POPS LF@%s\n",str3);
    printf("POPS LF@%s\n",str2);
    printf("CONCAT LF@%s LF@%s LF@%s\n",str1,str2,str3);
    printf("POPS LF@%s\n",str1);

}

///------------LABEL GENERATORS------------------------

void new_label_if(char *func_name, tScopeItem *item, int deep_index)
{

    printf("LABEL _%s_%d_%d_if\n", func_name, item->scopeLevel, deep_index);

}

void new_label_for(char *func_name, tScopeItem *item, int deep_index)
{

    printf("LABEL _%s_%d_%d_for\n", func_name, item->scopeLevel, deep_index);

}

///------------IF/ELSE FUNCTIONS------------------------

//void expr_in_if(expr)

void if_prefix(char *func_name, tScopeItem *item, int deep_index)
{

    printf("JUMPIFEQ _%s_%d_%d LF@if_bool_result bool@false\n",func_name, item->scopeLevel, deep_index);

}

void if_else(char *func_name, tScopeItem *item, int deep_index)
{
    
    printf("JUMP _%s_%d_%d\n",func_name, item->scopeLevel, ++deep_index);
    printf("# ELSE PART\n");
    new_label(func_name,item->scopeLevel,deep_index);

}

void if_suffix(char *func_name, tScopeItem *item, int deep_index)
{

    printf("# FI\n");
    new_label(func_name,item->scopeLevel,deep_index);
    
}

///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------

//ASK: Problem s rovnakym nazvom premennej(kazdej premennej index a deep??)
//only after for and if/else scopes
void all_vars_to_new_scope(tScopeItem *item, int deep_index, int vars_total)
{

    printf("# TRANSFER VARS TO NEW SCOPE\n");
    printf("CREATEFRAME\n");
    for(int i=0; i<vars_total; i++)
    {

        printf("DEFVAR TF@%s", vars_total[i]->name);
        printf("MOVE TF@%s %s@%s",get_var_type(vars_total[i]->type),
        vars_total[i]->value);

    }

}

//only after for and if/else scopes
void all_vars_after_new_scope(tScopeItem *item, int deep_index, int vars_total)
{

    printf("# TRANSFER VARS REAL VALUES\n");
    for(int i=0; i<vars_total; i++)
    {
    
        printf("MOVE LF@%s TF@%s\n");

    }
    printf("# TRANSFER VARS's VALUES END\n");

}

///--------------------FOR FUNCTIONS-------------------------------

void for_args_TF_declar(char *func_name, TItem type)
{

    //have to create before execute for loop
    printf("CREATEFRAME\n");

    printf("DEFVAR TF@i\n");
    printf("MOVE TF@i ");
    declared_vars_default_init(type);

    printf("DEFVAR TF@cond\n");
    printf("MOVE TF@cond ");
    declared_vars_default_init(type);

    printf("DEFVAR TF@incr\n");
    printf("MOVE TF@incr ");
    declared_vars_default_init(type);
      
}

void for_prefix(char *func_name, tScopeItem *item, int deep_index)
{

    printf("# START FOR IN %s\n", func_name);
    new_label(func_name,item->scopeLevel,deep_index);
    printf("PUSHFRAME\n");
    new_label_for(func_name,item->scopeLevel,deep_index);
    //nasleduje telo foru
}

void for_suffix(char *func_name, tScopeItem *item, int deep_index)
{

    //koniec tela foru,...cond
    printf("JUMPIFNEQ ");
    new_label_for(func_name,item->scopeLevel,deep_index);
    printf(" LF@i LF@cond\n");
    printf("POPFRAME\n");
    printf("# END FOR LOOP\n");

}