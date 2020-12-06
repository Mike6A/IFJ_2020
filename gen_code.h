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

typedef struct identifierListNode{
    char *name;
    struct identifierListNode *next;
    struct identifierListNode *prev;
} idListNode;
typedef struct identifierList{
    idListNode* first;
    idListNode* last;
    int scope;
    struct identifierList* prevScope;
} idList;


struct genExpr {
    char* value;
    char* type;
    bool sign;
    bool constant;
    bool alocated;
};

//function in gen_code.c

///------------SPECIAL CONVERTING FUNCS----------
int hex_to_dec(char hex[]);
void parse_str(char *str);

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
void program_exit(tExpReturnType *ret_err);

///----------USER FUNCTION's FUNCTIONS-------------

void general_func_call(char *func_name);
void general_func_prefix(char *func_name, tFuncItem *func);
void general_func_suffix(char *func_name);
void func_args_TF_declar(char *func_name, tFuncItem *func, SyntaxNodes* paramValues);
void no_built_in_func_args_TF_declar(char *func_name, tFuncItem *func, SyntaxNodes* paramValues);
void func_ret_declar(char *func_name, tFuncItem *func);
void func_ret_get_value(char *func_name, tFuncItem *func,SyntaxNodes* retValues);
void func_ret_to_LF(char *func_name,tFuncItem *func, SyntaxNodes* dest);

///-----------VARS & EXPRESSIONS FUNCTIONS------------------
void general_terminal_val(tToken token);
void declared_vars_default_init(TItem type);
char* get_var_type(TItem type);
struct genExpr GenParseExpr(SyntaxNode* root, char* assignTo, char* right, char* left, char* type);
void GENASIGN(SyntaxNode* root, tHashItem* item);
void vars_default_declar_init(SyntaxNode *root, tHashItem *item);
void vars_set_new_value(SyntaxNode *root, tHashItem *item);
void var_assign_to_func_value(char *func_name, tHashItem *item);

///------------LABEL GENERATORS------------------------
void label_if_else_end(char *func_name);
void label_for_end(char *func_name);

///------------IF/ELSE FUNCTIONS------------------------
void if_cond(SyntaxNode *root,char *func_name);
void if_prefix(char *func_name);
void if_suffix(char *func_name);
void else_prefix(char *func_name);
void else_suffix(char *func_name);

///--------------------FOR FUNCTIONS-------------------------------
void for_header();
void for_afterDeclaration(char* func_name);
void for_cond_to_loop(SyntaxNode *root,char *func_name);
void for_start_Assign();
void for_end_Assign();
void for_prefix(char *func_name);
void for_suffix(char *func_name);

///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------
void all_vars_to_new_scope();
void all_vars_after_new_scope();

#endif


