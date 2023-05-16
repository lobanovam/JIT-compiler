#include <assert.h>
#include <string.h>

#include "../includes/log.h"
#include "../includes/jit.h"
#include "../includes/translator.h"


#define EMIT(cmd, size)                               \
            log("EMITTING %s\n", #cmd);               \
            EmitCmd(X86_CODE, cmd, size);                    


#define GET_X86_SIZE(name)  SIZE_##name 

const int PAGESIZE = 4096;

void WriteImmedPush(X86_CODE_T* X86_CODE, int value);
void WriteImmedValue(X86_CODE_T* X86_CODE, int value);

void TranslateHLT(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE);
void TranslatePush(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE);
void TranslatePop(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateMath(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateCondJump(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateJumpCall(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateSqrt(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateIn(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateOut(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);



void Translate(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    size_t curIndex = 0;
    size_t cmdCt = IR_HEAD->nativeCmdCt;

    for (; curIndex < cmdCt; curIndex++) {

        IR_HEAD->currentIndex = curIndex;

        switch (IR_HEAD->ir_StructArr[curIndex].nativeNum) {

            case HLT:
                TranslateHLT(IR_HEAD, X86_CODE);
            break;

            case PUSH:
                TranslatePush(IR_HEAD, X86_CODE);
            break;

            case POP:
                TranslatePop(IR_HEAD, X86_CODE);
            break;

            case ADD:
            case SUB:
            case MUL:
            case DIV:
                TranslateMath(IR_HEAD, X86_CODE);
            break;

            case JA:
            case JAE:
            case JB:
            case JBE:
            case JE:
            case JNE:
                TranslateCondJump(IR_HEAD, X86_CODE);
            break;

            case JUMP:
            case CALL:
                TranslateJumpCall(IR_HEAD, X86_CODE);
            break;
            
            case SQRT:
                TranslateSqrt(IR_HEAD, X86_CODE);
            break;
            
            case IN:
                TranslateIn(IR_HEAD, X86_CODE);
            break;

            case OUT:
                TranslateOut(IR_HEAD, X86_CODE);
            break;

            case COPY:
            break;

            break;
        }
    }
}

void EmitCmd(X86_CODE_T* X86_CODE, u_int64_t cmd, int size) {

    *(u_int64_t *) (X86_CODE->BinaryCode + X86_CODE->curLen) = cmd;
    X86_CODE->curLen += size;

}



void TranslatePush(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    size_t curIndex = IR_HEAD->currentIndex;
    size_t nativeIp = IR_HEAD->ir_StructArr[curIndex].nativeIp;
    u_int32_t relAddr = 0;

    switch (IR_HEAD->ir_StructArr[curIndex].SpecArg.argType) {

        case IMMED:     //push 10
            WriteImmedPush(X86_CODE, IR_HEAD->byteCode[nativeIp + 1]);
        break;
        
        case REG:       //push ax

        break;
        
        case REG_IMMED: //push rax+10

        break;

        case RAM_IMMED: //push [10]
            relAddr = IR_HEAD->byteCode[nativeIp + 1]; 

        break;

        case RAM_REG:   //push[rax]

        break;

        case RAM_REG_IMMED: //push[rax+10]

        break;
        
        default:
        break;
        }


}

void WriteImmedPush(X86_CODE_T* X86_CODE, int value) {

    EMIT(MOV_REG_IMMED | (RAX << 8), GET_X86_SIZE(MOV_REG_IMMED));
    WriteImmedValue(X86_CODE, value);
    EMIT(PUSH_REG | (RAX << 8), GET_X86_SIZE(PUSH_REG));
}

void WriteImmedValue(X86_CODE_T* X86_CODE, int value) {

    *(int64_t *) (X86_CODE->BinaryCode + X86_CODE->curLen) = (int64_t) value;
    X86_CODE->curLen += sizeof(int64_t);

}

void TranslatePop(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);


}

void TranslateHLT(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

}

void TranslateMath(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);


}

void TranslateCondJump(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

}

void TranslateJumpCall(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

}

void TranslateSqrt(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);


}

void TranslateIn(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);


}

void TranslateOut(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);


}

void X86_CODE_CTOR(X86_CODE_T* X86_CODE, size_t totalSize) {

    assert(X86_CODE != NULL);
    log("#in X86_CODE_CTOR\n\n");

    X86_CODE->BinaryCode = (char*) aligned_alloc(PAGESIZE, totalSize + 1);
    assert(X86_CODE->BinaryCode != NULL);
    memset(X86_CODE->BinaryCode, totalSize + 1, x86_RET);

    X86_CODE->memSegment = NULL; // need to calc max ram index
    X86_CODE->curLen = 0;
    X86_CODE->totalSize = 0;

}