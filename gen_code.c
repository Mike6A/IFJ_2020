/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 */

#include <stdio.h>
#include "gen_code.h"

///---------BUILT-IN FUNCTIONS-----------------

/// built-in string functions

void bif_lenght()
{

    printf("# func len(s string) (int)\n");
    printf("LABEL _func_len\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@str_len\n");
    printf("STRLEN LF@str_len LF@arg_0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_substr()
{
    printf("# func substr(s string,i int,n int) (string, int)\n");
    printf("LABEL _func_substr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_substr\n");
    printf("MOVE LF@final_substr string@\n");
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
    printf("CONCAT LF@final_substr LF@final_substr LF@char_on_index\n");
    printf("ADD LF@index LF@index int@1\n");
    printf("ADD LF@loop_counter LF@loop_counter int@1\n");
    printf("JUMPIFNEQ _func_loop LF@loop_counter LF@arg_2\n");
    printf("WRITE LF@final_substr\n");
    printf("LABEL _func_error\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_ord()
{

    printf("# func ord(s string,i int) (int, int)\n");
    printf("LABEL _func_ord\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_int\n");
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_1 int@0\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("DEFVAR LF@max_int_value\n");
    printf("STRLEN LF@max_int_value LF@arg_0\n");
    printf("SUB LF@max_int_value LF@max_int_value int@1\n");
    printf("GT LF@in_string LF@arg_1 LF@max_int_value\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n" );
    printf("STRI2INT LF@final_int LF@arg_0 LF@arg_1\n" );
    printf("LABEL _func_error\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}


void bif_chr()
{

    printf("# func chr(i int) (string, int)\n");
    printf("LABEL _func_chr\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_char\n");
    printf("MOVE LF@final_char string@\n");
    printf("DEFVAR LF@in_string\n");
    printf("MOVE LF@in_string bool@true\n");
    printf("LT LF@in_string LF@arg_0 int@0\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("GT LF@in_string LF@arg_0 int@255\n");
    printf("JUMPIFEQ _func_error LF@in_string bool@true\n");
    printf("INT2CHAR LF@final_char LF@arg_0\n");
    printf("LABEL _func_error\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

///build-in functions for data type conversion

void bif_int2float()
{

    printf("# func int2float(i int) (float64 int)\n");
    printf("LABEL _func_int2float\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_float\n");
    printf("INT2FLOAT LF@final_float LF@arg_0\n");
    printf("DEFVAR LF@int2float_ret_1\n");
    printf("DEFVAR LF@int2float_ret_2\n");
    printf("MOVE LF@int2float_ret_1 LF@final_float\n");
    printf("MOVE Lf@int2float_ret_2 int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_float2int()
{

    printf("# func float2int(f float64) (int int)\n");
    printf("LABEL _func_float2int\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_int\n");
    printf("FLOAT2INT LF@final_int LF@arg_0\n");
    printf("DEFVAR LF@float2int_ret_1\n");
    printf("DEFVAR LF@float2int_ret_2\n");
    printf("MOVE LF@float2int_ret_1 LF@final_int\n");
    printf("MOVE LF@float2int_ret_2 int@0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

///built-in function for value output

void bif_print()
{

    printf("# func print (term1,term2,...,term𝑛)\n");


}

///built-in functions for read literates and print terms

void bif_inputs()
{

    printf("# func inputs() (string,int)\n");
    printf("LABEL _func_inputs\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@input\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@input string\n");
    printf("TYPE LF@input_type LF@input\n");
    printf("JUMPIFNEQ _func_ret LF@input_type string@string\n");

    printf("LABEL _func_ret\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_inputi()
{

    printf("# func inputi() (int,int)\n");
    printf("LABEL _func_inputi\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@input\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@input int\n");
    printf("TYPE LF@input_type LF@input\n");
    printf("JUMPIFNEQ _func_ret LF@input_type string@int\n");
    printf("WRITE LF@input\n");
    printf("WRITE string@\010\n");
    printf("LABEL _func_ret\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_inputf()
{

    printf("# func inputf() (float64,int)\n");
    printf("LABEL _func_inputf\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@input\n");
    printf("DEFVAR LF@input_type\n");
    printf("READ LF@input float\n");
    printf("TYPE LF@input_type LF@input\n");
    printf("JUMPIFNEQ _func_ret LF@input_type string@float\n");
    printf("WRITE LF@input\n");
    printf("WRITE string@\010\n");
    printf("LABEL _func_ret\n");
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

void program_exit(int err_code) //TODO USING
{

    printf("EXIT int@%d\n",err_code);
    printf("# END OF GEN_CODE\n");

}

void general_func_call(char *func_name)
{

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

void func_args_TF_declar(char *func_name, int param_count)
{

    //have to create before args pass(for non args func too)
    printf("CREATEFRAME\n");
    
    if(param_count > 0)
    {

        printf("# CREATE VARS FOR %s's ARGS\n",func_name);
        for(int i = 0; i<param_count; i++)
        {
            
            printf("DEFVAR TF@arg_%d\n",i);
            printf("MOVE TF@arg_%d ",i);
            declared_vars_default_init(type);

        }

        printf("# ALL VARS FOR ARGS HAS BEEN CREATED\n");

    } else
        printf("# FUNC %s IS WITHOUT ARGS\n",func_name);
      
}

void func_param_LF_declar(char *func_name, int param_count)
{
    
    if(param_count > 0)
    {

        printf("# CREATE VARS FOR %s's PARAMS\n",func_name);
        for(int i = 0; i<param_count; i++)
        {
            
            printf("DEFVAR LF@param_%d\n",i);
            printf("MOVE LF@param_%d ",i);
            declared_vars_default_init(type);

        }

        printf("# ALL VARS FOR PARAMS HAS BEEN CREATED\n");

    } else
        printf("# FUNC %s IS WITHOUT PARAMS\n",func_name);
    
}

///---------GENERATE INIT & VALUES FOR VARIABLES-----------------



int main()
{

    char *function_name = "TEST"; //TODO find way
    int parameter_counter = 2;
    int return_counter = 3;
    int error_code = 0; //TODO find waY
    type = TInt;

    program_start();
    main_prefix();

    func_args_TF_declar(function_name, parameter_counter);
    general_func_call(function_name);

    general_func_prefix(function_name);
    //func_param_LF_declar(function_name, parameter_counter);
   // func_ret_declar(function_name, return_counter);
    general_func_suffix(function_name);

    main_suffix();
    program_exit(error_code);

    return 0;
}