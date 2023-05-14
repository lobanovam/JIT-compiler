#include "../includes/language.h"
#include "../includes/translator.h"

struct Variable VARIABLES[MAX_VAR_AMOUNT] = {};
static int freeRAMindex = 0;

int Translate(struct Tree* tree) {
    logprint("\n\n-----------------start translating-----------------\n");

    SetVarARR(tree->root);
    logprint("setVarArr done\n");
    for (int i = 0; i < 5; i++) {
        printf("%s ", VARIABLES[i].var);
    }

    FILE* AsmFile = fopen("../CPU/myAsm/language.asm", "w");
    if (AsmFile == NULL) {
        printf("ERROR: can't open AsmFile\n");
        return 1;
    }

    WriteAsm(tree->root, AsmFile);

    return 0;

}

int WriteAsm(struct node* node, FILE* AsmFile) {
    if (!node) return 0;

    static int labelNumIf = 0;
    static int curLabelWhile = 0; 

    if (node->type == VAL_TYPE) {
        prAsm("push %d\n", CastToInt(node->value->dbl));

    } else if (node->type == NAME_TYPE) {
        prAsm("%s:\n", node->value->Name);
        GetParams(node->left, AsmFile);

    } else if (node->type == VAR_TYPE) {
        int index = GetVarIndex(node->value->Name);
        if (index < 0)
            printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
        prAsm("push [%d]\n", index);
    } else if (node->type == SPEC_TYPE) {

        int curLabelIf = -1;

        switch (node->value->langType) {

            case ST:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                break;

            case IS_EE:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("jne if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("jne while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case IS_NE:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("je if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("je while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case IS_BE:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("jb if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("jb while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case IS_GE:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("ja if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("ja while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case IS_GT:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("jae if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("jae while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case IS_BT:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (node->parent->value->langType == IF) {
                    prAsm("jbe if_label%d:\n\n", labelNumIf);
                } else if (node->parent->value->langType == WHILE) {
                    prAsm("jbe while_end_label%d:\n\n", curLabelWhile);
                }
                break;

            case WHILE:
                logprint("\n\nIN WHILE\n\n");
                curLabelWhile++;
                prAsm("\nwhile_label%d:\n", curLabelWhile);
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);

                prAsm("jump while_label%d:\n", curLabelWhile);
                prAsm("while_end_label%d:\n\n", curLabelWhile);

                break;

            case IF:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                break;

            case ELSE:
                curLabelIf = labelNumIf;
                labelNumIf++;

                WriteAsm(node->left, AsmFile);
                prAsm("jump else_label%d:\n", curLabelIf);

                prAsm("\nif_label%d:\n", curLabelIf);
                WriteAsm(node->right, AsmFile);

                prAsm("\nelse_label%d:\n", curLabelIf);
                break;
            

            case FUNC:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                if (!strcmp(node->left->value->Name, "main")) 
                    prAsm("hlt\n\n");
                break;

            case ADD:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                prAsm("add\n");
                break;
            
            case SUB:
                WriteAsm(node->right, AsmFile);
                WriteAsm(node->left, AsmFile);
                prAsm("sub\n");
                break;

            case MUL:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                prAsm("mul\n");
                break;

            case DIV:
                WriteAsm(node->right, AsmFile);
                WriteAsm(node->left, AsmFile);
                
                prAsm("div\n");
                break;

            case SQRT:
                WriteAsm(node->left, AsmFile);
                WriteAsm(node->right, AsmFile);
                prAsm("sqrt\n");
                break;

            case CALL:
                PushParams(node->left->left, AsmFile);
                prAsm("call %s:\n", node->left->value->Name);
                break;

            case PARAM:
                break;

            case IN:
                GetInput(node->left, AsmFile);
                break;

            case OUT:
                GetOutput(node->left, AsmFile);
                break;

            case RET:
                GetRet(node->left, AsmFile);
                prAsm("ret\n\n");
                break;

            case VAR:
                prAsm("\n");
                WriteAsm(node->right, AsmFile);
                prAsm("pop [%d]\n",  GetVarIndex(node->left->value->Name));
                break;

            case EQ:
                WriteAsm(node->right, AsmFile);
                prAsm("pop [%d]\n", GetVarIndex(node->left->value->Name));
                break;

            case NIL:
                break;

        }
    }
    return 0;
}

int GetParams(struct node* node, FILE* AsmFile) {
    while (node->value->langType == PARAM) {
        if (node->left->left->type == VAR_TYPE) {
            int index = GetVarIndex(node->left->left->value->Name);
            if (index < 0)
                printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
            prAsm("pop [%d]\n", index);
        } else {
            printf("ERROR: ONLY VAR CAN BE PARAMS on line %d\n", __LINE__);
        }
    
        node = node->right;
    }
    return 0;
}

int GetRet(struct node* node, FILE* AsmFile) {
    if (node->type == VAR_TYPE) {
        int index = GetVarIndex(node->value->Name);
        if (index < 0)
            printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
        prAsm("push [%d]\n", index);
    } else if (node->type == VAL_TYPE) {
        prAsm("push %d\n", CastToInt(node->value->dbl));
    }

    return 0;
}

int GetInput(struct node* node, FILE* AsmFile) {

    while (node->value->langType == PARAM) {

        if (node->left->type == VAR_TYPE) {
            int index = GetVarIndex(node->left->value->Name);
            if (index < 0)
                printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
            prAsm("in\n");
            prAsm("pop [%d]\n", index);
        } else {
            printf("ERROR: IN() INCORRECT PARAM on line %d\n", __LINE__);
        }
        node = node->right;
    }
}

int GetOutput(struct node* node, FILE* AsmFile) {
    while (node->value->langType == PARAM) {

        if (node->left->type == VAR_TYPE) {
            int index = GetVarIndex(node->left->value->Name);
            if (index < 0)
                printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
            prAsm("push [%d]\n", index);
            prAsm("out\n");
        } else if (node->left->type == VAL_TYPE) {
            prAsm("push %d\n", CastToInt(node->left->value->dbl));
            prAsm("out\n");
        } else {
            printf("ERROR: IN() INCORRECT PARAM on line %d\n", __LINE__);
        }
        node = node->right;
    }
}

int PushParams(struct node* node, FILE* AsmFile) {
    int curIndex = 0;
    int params[MAX_PARAMS] = {};
    while (node->value->langType == PARAM) {
        printf("trying to push params\n");

        if (node->left->type == VAR_TYPE) {
            int index = GetVarIndex(node->left->value->Name);
            if (index < 0)
                printf("ERROR: variable %s not found in VARIABLES[]\n", node->value->Name);
            params[curIndex++] = index;
        } else {
            printf("ERROR: ONLY VAR CAN BE PARAMS on line %d\n", __LINE__);
        }

        node = node->right;
    }
    for (int i = 0; i < curIndex; i++) {
        printf("param[%d] = %d\n", i, params[i]);
    }
    curIndex--;

    
    for (curIndex; curIndex >= 0; curIndex--) {
        prAsm("push [%d]\n", params[curIndex]);
        printf("pushed %d\n", params[curIndex]);
    }

    return 0;
}

int CastToInt(double dbl) {
    dbl *= PRECISION;
    return (int)dbl;
}

int GetVarIndex(char* var) {
    for (int i = 0; i < MAX_VAR_AMOUNT; i++) {
        if (!strcmp(var, VARIABLES[i].var))
            return VARIABLES[i].ramIndex;
    }
    return -1;
}

int SetVarARR(struct node* node) {
    if (node->type == VAR_TYPE && node->parent->type == SPEC_TYPE && node->parent->value->langType == VAR) {
        logprint("found var type\n");
        VARIABLES[freeRAMindex].ramIndex = freeRAMindex;
        logprint("first assignation done\n");
        VARIABLES[freeRAMindex].var = node->value->Name;

        freeRAMindex++;
    }
    if(node->left) SetVarARR(node->left);
    if(node->right) SetVarARR(node->right);

    return 0;
}