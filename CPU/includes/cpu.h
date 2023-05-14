#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "masks.h"
#include "stack.h"

const char* CPU_SIGNATURE = "CP";
const char* CODE = "./a_code.txt";
const size_t LEN_LINE = 30;

const size_t RAM_SIZE = 100;
const size_t REG_SIZE = 7;
const int PRECISION = 1000;

int Compile(struct CPU* cpu, size_t cmd_ct);
void clearBuff();

void PushArg(struct CPU* cpu);
void PopArg(struct CPU* cpu);

void CpuCtor(struct CPU* cpu, size_t cmd_ct);
void CpuDtor(struct CPU* cpu);

int checkSign(struct CPU* cpu, FILE* a_code);
size_t getCode(struct CPU* cpu);

void PrintRAM(int* RAM);

const int CMD_MASK = 31; /*11111*/

struct CPU {
    int* code;
    int* regs;
    int* RAM;
    const char* signature;
    size_t ip;
    struct Stack lastCall;
    struct Stack stk1;
};

enum CPU_ERRORS{
    WRONG_SIGNATURE = 1,
    UNDEFINED_CMD,
    DEV_BY_0,
    INVALID_REG_INDEX,
    INVALID_MEM_INDEX,
    NEG_SQRT
};



#endif // CPU_H_INCLUDED
