#include <assert.h>
#include <string.h>

#include "../includes/log.h"
#include "../includes/jit.h"
#include "../includes/translator.h"



#define FIRST_XMM(number)  (number << 27)
#define SECOND_XMM(number) (number << 24)
#define ARITHM(number)     (number << 8 )
#define FIRST_G(number)    (number << 16)
#define SECOND_G(number)   (number << 19)
#define J_MASK(number)     (number << 8 )
#define CVT_XMM(number)    (number << 35)
#define IMM_REG(number)    (number << 8 )
#define R_REG(number)      (number << 8 )

#define EMIT(cmd, size)                                                                 \
            log("EMITTING %s\n", #cmd);                                                 \
            EmitCmd(X86_CODE, cmd, size); 

#define LOAD_2_ARGS_IN_XMM0_XMM1                                                        \
             EMIT(CVTSI2SD_XMM0_RSP, GET_X86_SIZE(CVTSI2SD_XMM0_RSP));                  \
             EMIT(CVTSI2SD_XMM1_RSP_8, GET_X86_SIZE(CVTSI2SD_XMM1_RSP_8));              \
             EMIT(ADD_RSP_16, GET_X86_SIZE(ADD_RSP_16));

#define LOAD_1000_IN_XMMX(mask)                                                         \
             EMIT(MOV_REG_IMMED, GET_X86_SIZE(MOV_REG_IMMED));                          \
             WriteImmed64(X86_CODE, 1000);                                              \
             EMIT(CVTSI2SD_XMMF_RAX | CVT_XMM(mask), GET_X86_SIZE(CVTSI2SD_XMMF_RAX));

#define PUSH_FROM_XMM0                                                                  \
             EMIT(CVTSD2SI_RAX_XMMF | (XMM0 << 32), GET_X86_SIZE(CVTSD2SI_RAX_XMMF));   \
             EMIT(PUSH_REG | RAX, GET_X86_SIZE(PUSH_REG));

#define SET_MEM_IN_R15                                                                   \
             EMIT(MOV_R_REG_IMMED | R_REG(R15), GET_X86_SIZE(MOV_R_REG_IMMED));          \
             WriteImmed64(X86_CODE, (int64_t)X86_CODE->memSegment)

#define SET_STACK_IN_R14                                                                 \
             EMIT(MOV_R_REG_IMMED | R_REG(R14), GET_X86_SIZE(MOV_R_REG_IMMED))           \
             WriteImmed64(X86_CODE, (int64_t)X86_CODE->CallStackSegment);



#define GET_X86_SIZE(name)  SIZE_##name 

const int CONST_OFFSET = GET_X86_SIZE(MOV_R_REG_IMMED) +   // setting mem in r15
                         sizeof(u_int64_t)             +
                         GET_X86_SIZE(MOV_R_REG_IMMED) +   // setting stack in r14
                         sizeof(u_int64_t);

const int PAGESIZE        = 4096;
const int MEM_ALIGNMENT   = 4096;
const int MEM_SIZE        = 4096;
const int CALL_STACK_SIZE =  100;

void IntScanfWrap(int* num);
void DoublePrintfWrap(double num);

void WriteImmed64(X86_CODE_T* X86_CODE, int64_t value);
void WriteImmed32(X86_CODE_T* X86_CODE, int32_t value);

void TranslateRet(IR_HEAD_T* IR_HEAD,     X86_CODE_T* X86_CODE);
void TranslateHlt(IR_HEAD_T* IR_HEAD,     X86_CODE_T* X86_CODE);
void TranslatePushPop(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE);

void TranslateDiv(IR_HEAD_T* IR_HEAD,     X86_CODE_T* X86_CODE);
void TranslateMul(IR_HEAD_T* IR_HEAD,     X86_CODE_T* X86_CODE);
void TranslateAddSub(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);

void TranslateCondJump(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE);
void TranslateJump(IR_HEAD_T* IR_HEAD,      X86_CODE_T* X86_CODE);
void TranslateCall(IR_HEAD_T* IR_HEAD,      X86_CODE_T* X86_CODE);
void TranslateSqrt(IR_HEAD_T* IR_HEAD,      X86_CODE_T* X86_CODE);
void TranslateIn(IR_HEAD_T* IR_HEAD,        X86_CODE_T* X86_CODE);
void TranslateOut(IR_HEAD_T* IR_HEAD,       X86_CODE_T* X86_CODE);

void EmitCmd(X86_CODE_T* X86_CODE, u_int64_t cmd, int size);

void Translate(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n\nSTARTED TRANSLATING!\n\n");

    size_t curIndex = 0;
    size_t cmdCt = IR_HEAD->nativeCmdCt;

    SET_MEM_IN_R15;

    SET_STACK_IN_R14;

    for (; curIndex < cmdCt; curIndex++) {

        IR_HEAD->currentIndex = curIndex;

        switch (IR_HEAD->ir_StructArr[curIndex].nativeNum) {

            case RET:
                TranslateRet(IR_HEAD, X86_CODE);
            break;

            case HLT:
                TranslateHlt(IR_HEAD, X86_CODE);
            break;

            case PUSH:
            case POP:
                TranslatePushPop(IR_HEAD, X86_CODE);
            break;

            case ADD:
            case SUB:
                TranslateAddSub(IR_HEAD, X86_CODE);
            break;

            case MUL:
                TranslateMul(IR_HEAD, X86_CODE);
            break;

            case DIV:
                TranslateDiv(IR_HEAD, X86_CODE);
            break;

            case JA:
            case JAE:
            case JB:
            case JBE:
            case JE:
            case JNE:
                TranslateCondJump(IR_HEAD, X86_CODE);
            break;

            case CALL:
                TranslateCall(IR_HEAD, X86_CODE);
            break;

            case JUMP:
                TranslateJump(IR_HEAD, X86_CODE);
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

void TranslatePushPop(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslatePushPop\n\n");

    size_t curIndex = IR_HEAD->currentIndex;
    size_t nativeIp = IR_HEAD->ir_StructArr[curIndex].nativeIp;
    int nativeNum = IR_HEAD->ir_StructArr[curIndex].nativeNum;
    u_int32_t relMemAddr = 0;

    switch (IR_HEAD->ir_StructArr[curIndex].SpecArg.argType) {

        case IMMED:     //push 10 / pop 10
            if (nativeNum == PUSH) {
                EMIT(MOV_REG_IMMED | IMM_REG(RAX), GET_X86_SIZE(MOV_REG_IMMED));
                WriteImmed64(X86_CODE, (int64_t) IR_HEAD->byteCode[nativeIp + 1] * 1000);
                EMIT(PUSH_REG | IMM_REG(RAX), GET_X86_SIZE(PUSH_REG));
            }
            else 
                log("\t\t ERROR: cant't pop <immed>\n\n");

        break;
        

        case RAM_IMMED: //push [10] / pop [10]

            if (nativeNum == PUSH) {
                relMemAddr = IR_HEAD->byteCode[nativeIp + 1];
                EMIT(PUSH_R15_OFFSET, GET_X86_SIZE(PUSH_R15_OFFSET));
                WriteImmed32(X86_CODE, (int32_t) IR_HEAD->byteCode[nativeIp + 1] * 8);
            }
            else {
                EMIT(POP_R15_OFFSET, GET_X86_SIZE(POP_R15_OFFSET));
                WriteImmed32(X86_CODE, (int32_t) IR_HEAD->byteCode[nativeIp + 1] * 8);
            }

        break;

        case RAM_REG:   //push[rax]
            *(int*)rand() = rand();
        break;

        case RAM_REG_IMMED: //push[rax+10]
        break;

        case REG:       //push ax
        break;
        
        case REG_IMMED: //push rax+10
        break;
        
        default:
        break;
    }
}

void WriteImmed64(X86_CODE_T* X86_CODE, int64_t value) {

    log("\t immed64: %zu\n", value);

    *(int64_t *) (X86_CODE->BinaryCode + X86_CODE->curLen) =  value;
    X86_CODE->curLen += sizeof(int64_t);

}


void WriteImmed32(X86_CODE_T* X86_CODE, int32_t value) {

    log("\t immed32: %zu\n", value);
    *(int32_t *) (X86_CODE->BinaryCode + X86_CODE->curLen) = value;
    X86_CODE->curLen += sizeof(int32_t);

}

void TranslateHlt(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateHlt\n\n");

    EMIT(x86_RET, GET_X86_SIZE(x86_RET));

}

void TranslateRet(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateRet\n\n");

    EMIT(SUB_R14_8,    GET_X86_SIZE(SUB_R14_8));
    EMIT(PUSH_MEM_R14, GET_X86_SIZE(PUSH_MEM_R14));
    EMIT(x86_RET,      GET_X86_SIZE(x86_RET));

}

void TranslateDiv(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);
    
    log("\n#in TranslateDiv\n\n");

    LOAD_2_ARGS_IN_XMM0_XMM1;
    LOAD_1000_IN_XMMX(XMM2);

    EMIT(DIVPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM1), GET_X86_SIZE(DIVPD_XMMF_XMMS));
    EMIT(MULPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM2), GET_X86_SIZE(MULPD_XMMF_XMMS));

    PUSH_FROM_XMM0;

}

void TranslateMul(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateMul\n\n");

    LOAD_2_ARGS_IN_XMM0_XMM1;
    LOAD_1000_IN_XMMX(XMM2);
    

    EMIT(DIVPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM2), GET_X86_SIZE(DIVPD_XMMF_XMMS));
    EMIT(DIVPD_XMMF_XMMS | FIRST_XMM(XMM1) | SECOND_XMM(XMM2), GET_X86_SIZE(DIVPD_XMMF_XMMS));

    EMIT(MULPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM1), GET_X86_SIZE(MULPD_XMMF_XMMS));
    EMIT(MULPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM2), GET_X86_SIZE(MULPD_XMMF_XMMS));

    PUSH_FROM_XMM0;
}

void TranslateAddSub(IR_HEAD_T* IR_HEAD,  X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateAddSub\n\n");


    u_int64_t arithmMask = 0;

    if (IR_HEAD->ir_StructArr[IR_HEAD->currentIndex].nativeNum == ADD)
        arithmMask = ADD_MASK;
    else
        arithmMask = SUB_MASK;

    EMIT(POP_REG | RAX, GET_X86_SIZE(POP_REG));
    EMIT(POP_REG | RBX, GET_X86_SIZE(POP_REG));

    EMIT(ARITHM_REG_REG | ARITHM(arithmMask) | FIRST_G(RAX) | SECOND_G(RBX), GET_X86_SIZE(ARITHM_REG_REG));
    EMIT(PUSH_REG | RAX, GET_X86_SIZE(PUSH_REG));
}

void TranslateCondJump(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateCondJump\n\n");

    size_t curIndex   = IR_HEAD->currentIndex;
    int nativeNum     = IR_HEAD->ir_StructArr[curIndex].nativeNum;
    int32_t addrDiff  = IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.addrDiff;
    u_int64_t jmpMask = IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.jumpMask;

    EMIT(POP_REG | RAX,                              GET_X86_SIZE(POP_REG));
    EMIT(POP_REG | RBX,                              GET_X86_SIZE(POP_REG));
    EMIT(CMP_REG_REG | FIRST_G(RAX) | SECOND_G(RBX), GET_X86_SIZE(CMP_REG_REG));
    EMIT(x86_COND_JMP | J_MASK(jmpMask),             GET_X86_SIZE(x86_COND_JMP));
    WriteImmed32(X86_CODE, addrDiff);
}

void TranslateJump(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateJump\n\n");

    size_t curIndex = IR_HEAD->currentIndex;
    int32_t addrDiff = IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.addrDiff;

    EMIT(x86_JMP, GET_X86_SIZE(x86_JMP));
    WriteImmed32(X86_CODE, addrDiff);
}

void TranslateCall(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateCall\n\n");
    
    size_t curIndex  = IR_HEAD->currentIndex;
    log("in call cur index is %zd\n", curIndex);
    int32_t addrDiff = IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.addrDiff;

    size_t x86RelAddr = IR_HEAD->ir_StructArr[curIndex].SpecArg.x86RelAddr;
    log("in call x86RelAddr is %zd\n", x86RelAddr);
    size_t cmdSize    = IR_HEAD->ir_StructArr[curIndex].x86_Size;
    log("in call cmd size is %d\n", cmdSize);
    size_t AdrToContinue = x86RelAddr + cmdSize + CONST_OFFSET;
    
    u_int64_t absAdr = (u_int64_t)(X86_CODE->BinaryCode + AdrToContinue);

    EMIT(MOV_REG_IMMED | IMM_REG(RAX), GET_X86_SIZE(MOV_REG_IMMED));
    WriteImmed64(X86_CODE, absAdr);
    EMIT(MOV_MEM_R14_RAX, GET_X86_SIZE(MOV_MEM_R14_RAX));
    EMIT(ADD_R14_8, GET_X86_SIZE(ADD_R14_8));
    EMIT(x86_JMP, GET_X86_SIZE(x86_JMP));
    WriteImmed32(X86_CODE, addrDiff);
}

void TranslateSqrt(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateSqrt\n\n");

    EMIT(POP_REG | RAX, GET_X86_SIZE(POP_REG));
    EMIT(CVTSI2SD_XMMF_RAX | CVT_XMM(XMM0), GET_X86_SIZE(CVTSI2SD_XMMF_RAX));

    LOAD_1000_IN_XMMX(XMM1);

    EMIT(DIVPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM1), GET_X86_SIZE(DIVPD_XMMF_XMMS));
    EMIT(SQRTPD_XMM0_XMM0,                                     GET_X86_SIZE(SQRTPD_XMM0_XMM0));
    EMIT(MULPD_XMMF_XMMS | FIRST_XMM(XMM0) | SECOND_XMM(XMM1), GET_X86_SIZE(MULPD_XMMF_XMMS));

    PUSH_FROM_XMM0;  
}

void TranslateIn(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateIn\n\n");

    size_t curIndex = IR_HEAD->currentIndex;
    size_t x86RelAddr = IR_HEAD->ir_StructArr[curIndex].SpecArg.x86RelAddr;
    size_t cmdSize = IR_HEAD->ir_StructArr[curIndex].x86_Size;

    size_t AdrToContinue = x86RelAddr + cmdSize - GET_X86_SIZE(MOV_RSP_RBP) - 
                                                  GET_X86_SIZE(POPA)        + 
                                                  CONST_OFFSET;

    int32_t relAddr = (u_int64_t) IntScanfWrap - (u_int64_t)(X86_CODE->BinaryCode + AdrToContinue);
    
    EMIT(SUB_RSP_8,   GET_X86_SIZE(SUB_RSP_8));   // reserving space for input
    EMIT(LEA_RDI_RSP, GET_X86_SIZE(LEA_RDI_RSP));
    EMIT(PUSHA,       GET_X86_SIZE(PUSHA));
    EMIT(MOV_RBP_RSP, GET_X86_SIZE(MOV_RBP_RSP));
    EMIT(ALIGN_STACK, GET_X86_SIZE(ALIGN_STACK));

    EMIT(x86_CALL, GET_X86_SIZE(x86_CALL));
    WriteImmed32(X86_CODE, relAddr);

    EMIT(MOV_RSP_RBP,  GET_X86_SIZE(MOV_RBP_RSP));
    EMIT(POPA, GET_X86_SIZE(POPA));
}

void IntScanfWrap(int* num) {
    printf("Enter an integer number: ");
    scanf("%d", num);
    *num *= 1000;
}

void DoublePrintfWrap(double num) {
    printf("OUTPUT: %.3lf\n", num / 1000);
    num *= 2;
}

void TranslateOut(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE) {

    assert(IR_HEAD != NULL);
    assert(X86_CODE != NULL);

    log("\n#in TranslateOut\n\n");

    size_t curIndex = IR_HEAD->currentIndex;
    size_t x86RelAddr = IR_HEAD->ir_StructArr[curIndex].SpecArg.x86RelAddr;
    size_t cmdSize = IR_HEAD->ir_StructArr[curIndex].x86_Size;
    size_t AdrToContinue = x86RelAddr + cmdSize - GET_X86_SIZE(MOV_RSP_RBP)     - 
                                                  GET_X86_SIZE(POPA)            - 
                                                  GET_X86_SIZE(ADD_RSP_8)       + 
                                                  CONST_OFFSET;

    int32_t addrDiff = (u_int64_t) DoublePrintfWrap - (u_int64_t)(X86_CODE->BinaryCode + AdrToContinue);

    EMIT(CVTSI2SD_XMM0_RSP, GET_X86_SIZE(CVTSI2SD_XMM0_RSP));
    EMIT(PUSHA,             GET_X86_SIZE(PUSHA));
    EMIT(MOV_RBP_RSP,       GET_X86_SIZE(MOV_RBP_RSP));
    EMIT(ALIGN_STACK,       GET_X86_SIZE(ALIGN_STACK));

    EMIT(x86_CALL, GET_X86_SIZE(x86_CALL));
    WriteImmed32(X86_CODE, addrDiff);

    EMIT(MOV_RSP_RBP, GET_X86_SIZE(MOV_RSP_RBP));
    EMIT(POPA,        GET_X86_SIZE(POPA));
    EMIT(ADD_RSP_8,   GET_X86_SIZE(ADD_RSP_8));
}

void X86_CODE_CTOR(X86_CODE_T* X86_CODE, size_t totalSize) {

    assert(X86_CODE != NULL);
    log("\n#in X86_CODE_CTOR\n\n");

    size_t FullSize = totalSize + CONST_OFFSET;

    X86_CODE->BinaryCode = (char*) aligned_alloc(PAGESIZE, (FullSize * 2) * sizeof(char));
    log("done code calloc\n");
    assert(X86_CODE->BinaryCode != NULL);
    memset(X86_CODE->BinaryCode, FullSize * 2, x86_RET);
    log("done memset\n");

    log("trying to calloc mem\n");
    X86_CODE->memSegment = (char*) aligned_alloc(MEM_ALIGNMENT, MEM_SIZE * sizeof(char));
    log("done mem calloc\n");
    assert(X86_CODE->memSegment != NULL);
    memset(X86_CODE->memSegment, MEM_SIZE, 0xaa);

    X86_CODE->CallStackSegment = (u_int64_t*) aligned_alloc(MEM_ALIGNMENT, CALL_STACK_SIZE * (sizeof(u_int64_t)));
    assert(X86_CODE->CallStackSegment != NULL);

    X86_CODE->curLen = 0;
    X86_CODE->totalSize = FullSize;

}

void X86_CODE_DTOR(X86_CODE_T* X86_CODE) {

    assert(X86_CODE != NULL);

    log("\nin X86_CODE_DTOR\n\n");

    free(X86_CODE->BinaryCode);
    free(X86_CODE->memSegment);
    free(X86_CODE->CallStackSegment);

    X86_CODE->curLen    = 0;
    X86_CODE->totalSize = 0;


}