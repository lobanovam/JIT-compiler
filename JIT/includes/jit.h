#ifndef JIT_H_INCLUDED
#define JIT_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "../../CPU/includes/masks.h"

const int CMD_MASK = 31; /*11111*/

#define DEF_CMD(name, num, args, ...) name = num, 

enum CmdEnum {

    #include "../../CPU/includes/cmd.h"

};

#undef DEF_CMD

typedef struct ir_command {

    const char* name;
    int nativeNum;
    int hasArgs;

    size_t nativeIp;
    int nativeSize;
    int x86_Size;

    size_t SPECIAL_ARG;           // if cmd is push/pop - stores the arg type
                                  // if cmd is jump of any sort - stores next ip  

} ir_command; 


typedef struct IR_HEAD_T {

    size_t nativeCmdCt;
    int* byteCode;
    ir_command* ir_StructArr;
    size_t currentIp; 
    size_t currentIndex;

} IR_HEAD_T; 


enum CMD_TYPES {

    IMMED = ARG_IMMED,

    REG = ARG_REG,
    REG_IMMED = ARG_REG | ARG_IMMED,

    RAM_IMMED = ARG_RAM | ARG_IMMED,
    RAM_REG = ARG_RAM | ARG_REG,
    RAM_REG_IMMED = RAM_REG | ARG_IMMED,
   

} ;

typedef struct InstructionSize {



} InstructionSize;

InstructionSize instrSizes[] = {


};

void SetIR(IR_HEAD_T* IR_HEAD);
void IR_CTOR(IR_HEAD_T* IR_HEAD, size_t cmdCt);
void IR_DTOR(IR_HEAD_T*);
void readByteCode(FILE* byteCode, IR_HEAD_T* IR_HEAD);
size_t getCodeSize(FILE* byteCode);
void SetIrCommand(IR_HEAD_T* IR_HEAD, const char* name, int arg);
void HandleArgsCmd(IR_HEAD_T* IR_HEAD);
void IR_Dump(IR_HEAD_T* IR_HEAD);
int isJump(int num);
int isPushPop(int num);
char* getArgType(int argType);

#endif