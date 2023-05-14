#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

void readFile(struct Text* text);
void trimSpaces(struct Text* text);

int Tokenize(struct Token **tokens, struct Text* text);
char* getFuncName(const char* str);
char* getVarName(const char* str);
double getVal(const char* str, int* digits);
int getOp1(int ch);
int getOp2(int ch1, int ch2);


struct Token* CreateToken(int type, int LangOp, double dbl, char* name, size_t line);

void tokenDump(struct Token **tokens, size_t tokenSize);
void AstDump(struct node* node);
void DrawTree(struct node* node,  FILE* DumpAST);
void AstDump(struct node* node);



#endif