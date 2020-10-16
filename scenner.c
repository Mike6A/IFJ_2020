#include "scenner.h"

char actualChar;
char *actualString;

void getNextChar() {
    actualChar = getc(stdin);
}

bool isActLetter(){
    return ((actualChar > 'a' && actualChar < 'z') || (actualChar > 'A' && actualChar < 'Z') || actualChar == '_') ? true : false;
}

bool isActNumber(){
    return (actualChar > '0' && actualChar < '9') ? true : false;
}

tToken getToken() {
    getNextChar();
}