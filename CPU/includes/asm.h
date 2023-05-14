#ifndef ASM_H_INCLUDED
#define ASM_H_INCLUDED

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "masks.h"


const char* CODE = "a_code.txt";


void connectPointers(struct Text* text_ptr);
void readFile(struct Text* text);
void skipSpace(char** ptr);
void countLines(struct Text* text);
void WriteCode(int* code, int ip);

int SetCode(struct Text* text, int* code, size_t* ip, struct LABEL* labels);
int GetArg(struct Text* text, int* code, size_t* ip, int num, char* curPtr, struct LABEL* labels);
void SetRegNum(int* code, size_t* ip, char* reg_name);
void SetLabel(struct LABEL* labels, char* cmd, size_t* ip, int* label_ct);
int GetLabelNumByName(struct LABEL* labels, char* label_name);

const int CMD_LEN = 30;
const char* ASM_SIGNATURE = "CP";

const size_t LABEL_NUM_MAX = 30;
const size_t LABEL_NAME_LEN = 30;

struct LABEL {
    int value;
    char  Name[LABEL_NAME_LEN];
};

struct Text {
    char *buff;
    char **pointers;
    size_t ch_count;
    size_t lines_count;
};



#endif // ASM_H_INCLUDED
