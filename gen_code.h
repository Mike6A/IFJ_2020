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
#include "stack.h"
#include "stdarg.h"
#include <malloc.h>

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

typedef struct printListNode{
    char *printString;
    bool func_defined;
    bool defvarFunc;
    char *func_name;
    struct printListNode *next;
} pNode;

typedef struct printList{
    pNode* first;
    pNode* last;
} pList;

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
void bif_inputs();
void bif_inputi();
void bif_inputf();

///----------BASIC PROGRAM FUNCTIONS---------
void program_start();
void main_prefix();
void main_suffix();

///----------USER FUNCTION's FUNCTIONS-------------

void general_func_call(char *func_name);
void general_func_prefix(char *func_name, tFuncItem *func);
void general_func_suffix(char *func_name);
void func_args_TF_declar(char *func_name, tHashItem *funcItem, SyntaxNodes* paramValues);
void func_ret_declar(char *func_name, tFuncItem *func);
void func_ret_get_value(char *func_name, tFuncItem *func,SyntaxNodes* retValues);
void func_ret_to_LF(char *func_name,tFuncItem *func, SyntaxNodes* dest);

///-----------VARS & EXPRESSIONS FUNCTIONS------------------
void declared_vars_default_init(TItem type);
char* get_var_type(TItem type);
struct genExpr GenParseExpr(SyntaxNode* root, char* assignTo, char* right, char* left, char* type);
void vars_default_declar_init(SyntaxNode *root, tHashItem *item);
void vars_set_new_value(SyntaxNode *root, tHashItem *item);

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
void for_suffix(char *func_name);

///----------------BEFORE/AFTER FOR OR IF/ELSE SCOPE--------------------------
void all_vars_to_new_scope();
void all_vars_after_new_scope();

int getErrorList();
void printAndDeleteGenCode();
void deleteIdListGlobal();
void deletepList();
#endif


