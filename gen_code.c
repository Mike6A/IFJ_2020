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

int main()
{

    bif_lenght();
    bif_ord();
    bif_chr();
    bif_int2float();
    bif_float2int();


    return 0;
}