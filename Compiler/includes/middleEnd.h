#ifndef MIDDLE_H
#define MIDDLE_H
#include <stdio.h>

int WriteStandartTree(struct node* node);
int WriteNode(struct node* node, FILE* Standart, int spaces);

void PutVal(FILE* Standart, double dbl);
void PutVar(FILE* Standart, char *name);
void PutSpec(FILE* Standart, int SpecType);

int ReadStandartTree(struct Tree* tree);
struct node* ReadTree(struct node* parent, FILE* Standart, int side);

int GetOp(char* str);

void printSpaces(int spaceCt, FILE* Standart);

struct node* CreateNode(int type, int LangOp, double dbl, char *name, struct node* nodeL, struct node* nodeR);

const int LEFT = 1;
const int RIGHT = 2;

/*#define GET_OP(str)           \
    if (!strcmp(str, #str))   \
        return #str;          */


#endif