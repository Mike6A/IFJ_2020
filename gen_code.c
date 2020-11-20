/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 */

#include <stdio.h>

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

    printf("# func int2float(i int) (float64)\n");
    printf("LABEL _func_int2float\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_float\n");
    printf("INT2FLOAT LF@final_float LF@arg_0\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}

void bif_float2int()
{

    printf("# func float2int(f float64) (int)\n");
    printf("LABEL _func_float2int\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@final_int\n");
    printf("FLOAT2INT LF@final_int LF@arg_0\n");
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
    printf("JUMPIFNEQ _func_error LF@input_type string@string\n");
    printf("WRITE LF@input\n");
    printf("WRITE string@\010\n");
    printf("LABEL _func_error\n");
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
    printf("JUMPIFNEQ _func_error LF@input_type string@int\n");
    printf("WRITE LF@input\n");
    printf("WRITE string@\010\n");
    printf("LABEL _func_error\n");
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
    printf("JUMPIFNEQ _func_error LF@input_type string@float\n");
    printf("WRITE LF@input\n");
    printf("WRITE string@\010\n");
    printf("LABEL _func_error\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

}



int main()
{

    bif_inputs();
    bif_inputi();
    bif_inputf();


    return 0;
}