/**
 * @file stringBuilder.h
 * @author Michal Mlčoch (xmlcoc12@stud.fit.vutbr.cz)
 * @brief Header file for string builder.
 */
#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

/**
 * @brief String builder struct
 */
typedef struct {
    char* value; /**< string building variable */
    unsigned len; /**< length of actual string */
    unsigned allocated; /**< how many byts are alloced in value */
} tStringBuilder;

int initStringBuilder(tStringBuilder* builder);
void destructBuilder(tStringBuilder* builder);
int appendChar(tStringBuilder* builder, char c);
void cleanBuilder(tStringBuilder* builder);
int getStringFromBuilder(tStringBuilder* builder, char** dest);
int getStringAndCleanBuilder(tStringBuilder* builder, char** dest);
#endif