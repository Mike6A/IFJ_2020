/**
 * @file gen_code.c
 *
 * @brief Implementation of code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 */

#include <stdio.h>

/// built-in string functions
#define BIF_LENGTH 
do{\
    "# func len(s string) (int)\n"
    "LABEL _func_len\n"
    "PUSHFRAME\n"
    "DEFVAR LF@str_len\n"
    "STRLEN LF@str_len LF@arg_0\n"
    "POPFRAME\n"
    "RETURN\n"
}while(0)

#define BIF_ORD
do{\
    "# func ord(s string,i int) (int, int)\n");
    "LABEL _func_ord"
    "PUSHFRAME"
    "DEFVAR LF@final_int"
    "DEFVAR LF@in_string"
    "MOVE LF@in_string bool@true"
    "LT LF@in_string LF@arg_1 int@0"
    "JUMPIFEQ _func_error LF@in_string bool@true"
    "DEFVAR LF@max_int_value"
    "STRLEN LF@max_int_value LF@arg_0"
    "SUB LF@max_int_value LF@max_int_value int@1"
    "GT LF@in_string LF@arg_1 LF@max_int_value" 
    "JUMPIFEQ _func_error LF@in_string bool@true" 
    "STRI2INT LF@final_int LF@arg_0 LF@arg_1" 
    "LABEL _func_error"
    "POPFRAME"
    "RETURN"
}while(0)

#define BIF_CHR
do{\

    "# func chr(i int) (string, int)\n"
    "LABEL _func_chr\n"
    "PUSHFRAME\n"
    "DEFVAR LF@final_char\n"
    "MOVE LF@final_char string@\n"
    "DEFVAR LF@in_string\n"
    "MOVE LF@in_string bool@true\n"
    "LT LF@in_string LF@arg_0 int@0\n"
    "JUMPIFEQ _func_error LF@in_string bool@true\n"
    "GT LF@in_string LF@arg_0 int@255\n" 
    "JUMPIFEQ _func_error LF@in_string bool@true\n" 
    "INT2CHAR LF@final_char LF@arg_0\n" 
    "LABEL _func_error\n"
    "POPFRAME\n"
    "RETURN\n"
}while(0)


int main()
{

    return 0;
}