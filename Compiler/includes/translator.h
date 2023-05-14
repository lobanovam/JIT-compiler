#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "stdio.h"

const int MAX_VAR_AMOUNT = 60;
const int MAX_PARAMS = 10;
const int PRECISION = 1;


int Translate(struct Tree* tree);
int WriteAsm(struct node* node, FILE* AsmFile);
int SetVarARR(struct node* node);

int PushParams(struct node* node, FILE* AsmFile);
int GetInput(struct node* node, FILE* AsmFile);
int GetOutput(struct node* node, FILE* AsmFile);
int GetRet(struct node* node, FILE* AsmFile);
int GetParams(struct node* node, FILE* AsmFile);

int CastToInt(double dbl);
int GetVarIndex(char* var);

struct Variable {
    char* var;
    int ramIndex;
};


#define prAsm(str...)                                                 \
    fprintf(AsmFile, str);                                            \
    fflush(AsmFile)   

#endif
