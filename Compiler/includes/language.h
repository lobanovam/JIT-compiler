#ifndef LANG_H_INCLUDED
#define LANG_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dictionary.h"

#include "dump.h"
#include "log.h"

struct Tree {
    struct node* root;
};


char* getSpecName(int spec);
void TreeCtor(struct Tree* tree);
void NodesDtor(struct node* node);
void TreeDtor(struct Tree* tree);

enum LENS {
    MAX_FUNC_NAME = 50,
    MAX_VAR_NAME = 30,
};

struct Text {
    char *buff;
    size_t ch_count;
    size_t lines_count;
};

struct node {
    int               type;
    union LangUnion* value;
    struct node*    parent;
    struct node*      left;
    struct node*     right;
};

union LangUnion {
    double      dbl;
    int    langType;
    char*      Name;
};

struct Token {
    int               type;
    union LangUnion* value;
    size_t            line;
};

enum Types {
    VAL_TYPE,
    VAR_TYPE,
    SPEC_TYPE,
    NAME_TYPE,
};

enum LangTypes {
    UNKNOWN,
    ADD,
    SUB,
    DIV,
    MUL,
    SQRT,
    POW,
    AND,
    OR,
    EQ,

    // Comparison
    IS_EE,
    IS_GE,
    IS_BE,
    IS_GT,
    IS_BT,
    IS_NE,

    //userIO
    IN,
    OUT,
    
    // Special nodes 
    ST,
    IF,
    ELSE,
    NIL,
    VAR,
    WHILE,
    FUNC,
    INT_FUNC,
    VOID_FUNC,
    TYPE,
    VOID,
    RET,
    CALL,
    PARAM,
    OPEN_BR,
    CLOSE_BR,

    //for Tokenizer
    SoFOO,       //start of func
    EoFOO,       //end of func
    SoCOND,      //start of condition
    EoCOND,       //end of condition
    EOT,          //end of tokens
    EoPAR,        //end of params
    ENDIF         //endif   
};

#endif