#ifndef PARS_H_INCLUDED
#define PARS_H_INCLUDED

#include "DSL.h"

struct node* GetG(struct Token **tokens);
struct node* GetE(struct Token **tokens);
struct node* GetT(struct Token **tokens);
struct node* GetD(struct Token **tokens);
struct node* GetP(struct Token **tokens);
struct node* GetN(struct Token **tokens);


struct node* GetGrammar(struct Token **tokens);
struct node* GetExpression(struct Token **tokens);
struct node* GetCondition(struct Token **tokens);
struct node* GetComp(struct Token **tokens);
struct node* GetVariable(struct Token **tokens);
struct node* GetAssign(struct Token **tokens);
struct node* GetFunc(struct Token **tokens);
struct node* GetGeneralParams(struct Token **tokens);
struct node* GetRet(struct Token **tokens);
struct node* GetIf(struct Token **tokens);
struct node* GetArgs(struct Token **tokens);
struct node* GetIO(struct Token **tokens, int IO);

void ConnectNodes(struct node* node, struct node* nodeL, struct node* nodeR);

int isComp(int op);

struct node* CreateNode(int type, int LangOp, double dbl, char *name, struct node* nodeL, struct node* nodeR);

#endif