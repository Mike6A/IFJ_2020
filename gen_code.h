/**
 * @file gen_code.h
 *
 * @brief Header file for code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 */

#ifndef GEN_CODE_H
#define GEN_CODE_H

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "semantics.h"
#include "analyse.h"

#define MAX_LEN 20
#define MAX_STACK 100

typedef struct t_SN_Stack {
    SyntaxNode* node[MAX_STACK];
    int top;
} tSN_Stack;

struct genExpr {
    char* value;
    char* type;
    bool sign;
    bool constant;
};

void init_SN_Stack(tSN_Stack* list);
int add_SN_StackItem(tSN_Stack* list, SyntaxNode* node);
int destroy_SN_Stack(tSN_Stack* list);
bool is_SN_Stack_Empty(tSN_Stack* stack);
SyntaxNode* Pop_SN_Stack(tSN_Stack* list);

//function in gen_code.c

///------------BUILT-IN FUNCTIONS---------------
void bif_lenght();
void bif_substr();
void bif_ord();
void bif_chr();
void bif_int2float();
void bif_float2int();
void bif_print(SyntaxNodes* my_statement);
void bif_print_input(SyntaxNodes* my_statement);
void bif_inputs();
void bif_inputi();
void bif_inputf();

///----------BASIC PROGRAM FUNCTIONS---------
void program_start();
void main_prefix();
void main_suffix();
void program_exit(tExpReturnType ret_err);

///----------USER FUNCTION's FUNCTIONS-------------

void general_func_call(char *func_name);
void general_func_prefix(char *func_name);
void general_func_suffix(char *func_name);
void func_args_TF_declar(char *func_name, tFuncItem *func);
void func_ret_declar(char *func_name, tFuncItem *func);
void func_ret_get_value(char *func_name, tFuncItem *func);
void func_ret_to_LF(char *func_name,tFuncItem *func);
void func_ret_before_declar(char *func_name, tFuncItem *func);

///-----------VARS & EXPRESSIONS FUNCTIONS------------------
void general_terminal_val(tToken token);
void declared_vars_default_init(TItem type);
char* get_var_type(TItem type);
struct genExpr GenParseExpr(SyntaxNode* root, char* assignTo, char* right, char* left, char* type);
void GENASIGN(SyntaxNode* root, tHashItem* item);
void vars_default_declar_init(SyntaxNode *root, tHashItem *item);
void vars_set_new_value(SyntaxNode *root, tHashItem *item);

///---------STACK FUNCTIONS----------------------
void stack_concat_string(char *str3, char *str2, char *str1);
///------------LABEL GENERATORS------------------------
void new_label_for(char *func_name, tScopeItem *item, int deep_index);
void new_label_for_in(char *func_name, tScopeItem *item, int deep_index);
void new_label_if(char *func_name, tScopeItem *item, int deep_index);
void new_label_else(char *func_name, tScopeItem *item, int deep_index);

///------------IF/ELSE FUNCTIONS------------------------
void if_prefix(char *func_name, tScopeItem *item, int deep_index);
void if_else(char *func_name, tScopeItem *item, int deep_index);

///--------------------FOR FUNCTIONS-------------------------------
void for_args_TF_declar(char *func_name, TItem type);
void for_prefix(char *func_name, tScopeItem *item, int deep_index);
void for_suffix(char *func_name, tScopeItem *item, int deep_index);

///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------
void all_vars_to_new_scope(tScopeItem *item, int deep_index, int vars_total);
void all_vars_after_new_scope(tScopeItem *item, int deep_index, int vars_total);

#endif


