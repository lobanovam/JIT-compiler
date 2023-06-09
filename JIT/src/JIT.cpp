#include <assert.h>
#include <string.h>

#include "../includes/log.h"
#include "../includes/jit.h"
#include "../includes/translator.h"

void IR_CTOR(IR_HEAD_T* IR_HEAD, size_t cmdCt);
void IR_DTOR(IR_HEAD_T*);
int GetX86cmdSize(int nativeNum);
u_int64_t getJumpMask(int num);

void SetIR(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);

    log("\n#in SetIR\n\n");

    size_t CmdCt = IR_HEAD->nativeCmdCt;
    log("CMD_CT IS %zu\n", CmdCt);

    #define DEF_CMD(name, num, arg, ...)                                 \
                case num:                                                \
                    SetIrCommand(IR_HEAD, #name, arg) ;                  \
                break;

            
    while (IR_HEAD->currentIp < IR_HEAD->nativeCmdCt) {

        switch (IR_HEAD->byteCode[IR_HEAD->currentIp] & CMD_MASK) {

            #include "../../CPU/includes/cmd.h";

        }
    }

    IR_HEAD->nativeCmdCt = IR_HEAD->currentIndex;

    #undef DEF_CMD

}

void SetIrCommand(IR_HEAD_T* IR_HEAD, const char* name, int hasArgs) {

    assert(IR_HEAD != NULL);

    log("\n#in SetIrCommand cmd[%d] is %s\n",IR_HEAD->currentIp, name);

    size_t nativeIp = IR_HEAD->currentIp;
    size_t curIndex = IR_HEAD->currentIndex;
    size_t nativeNum = IR_HEAD->byteCode[IR_HEAD->currentIp] & CMD_MASK;

    IR_HEAD->ir_StructArr[curIndex].name      = name;
    IR_HEAD->ir_StructArr[curIndex].hasArgs   = hasArgs;
    IR_HEAD->ir_StructArr[curIndex].nativeIp  = nativeIp;
    IR_HEAD->ir_StructArr[curIndex].nativeNum = nativeNum;

    if (hasArgs) {
        log("\t %s has args\n", name);
        HandleArgsCmd(IR_HEAD);
    }
    else {
        log("\t %s has no args\n", name);
        IR_HEAD->ir_StructArr[curIndex].nativeSize = 1;
        IR_HEAD->ir_StructArr[curIndex].x86_Size = GetX86cmdSize(nativeNum);
    }

    if (nativeNum == IN || nativeNum == OUT || nativeNum == CALL) {
        if (nativeNum == CALL)
            log("\n getRelAddr for CALL, curIndex = %zd!\n", curIndex);
        IR_HEAD->ir_StructArr[curIndex].SpecArg.x86RelAddr = getRelAddr(IR_HEAD, nativeIp);
    }

    IR_HEAD->currentIp += IR_HEAD->ir_StructArr[curIndex].nativeSize;
    IR_HEAD->currentIndex++;
}

int GetX86cmdSize(int nativeNum) {

    log("#in GetX86cmdSize\n\n");

    switch (nativeNum) {

        case HLT:
            return GET_X86_SIZE(x86_RET);

        case RET:
            return GET_X86_SIZE(SUB_R14_8)    +
                   GET_X86_SIZE(PUSH_MEM_R14) +
                   GET_X86_SIZE(x86_RET);

        case ADD:
        case SUB:
            return GET_X86_SIZE(POP_REG)         +
                   GET_X86_SIZE(POP_REG)         +
                   GET_X86_SIZE(ARITHM_REG_REG)  +
                   GET_X86_SIZE(PUSH_REG);

        case DIV:
            return GET_X86_SIZE(CVTSI2SD_XMM0_RSP)      +
                   GET_X86_SIZE(CVTSI2SD_XMM1_RSP_8)    +
                   GET_X86_SIZE(ADD_RSP_16)             +
                   GET_X86_SIZE(DIVPD_XMMF_XMMS)        +              
                   GET_X86_SIZE(MOV_REG_IMMED)          +
                   sizeof(u_int64_t)                    +
                   GET_X86_SIZE(CVTSI2SD_XMMF_RAX)      +
                   GET_X86_SIZE(MULPD_XMMF_XMMS)        +
                   GET_X86_SIZE(CVTSD2SI_RAX_XMMF)      +
                   GET_X86_SIZE(PUSH_REG);

        case MUL:
            return GET_X86_SIZE(CVTSI2SD_XMM0_RSP)      +
                   GET_X86_SIZE(CVTSI2SD_XMM1_RSP_8)    +
                   GET_X86_SIZE(ADD_RSP_16)             +
                   GET_X86_SIZE(MOV_REG_IMMED)          +
                   sizeof(u_int64_t)                    +
                   GET_X86_SIZE(CVTSI2SD_XMMF_RAX)      +
                   GET_X86_SIZE(DIVPD_XMMF_XMMS)        +
                   GET_X86_SIZE(DIVPD_XMMF_XMMS)        +
                   GET_X86_SIZE(MULPD_XMMF_XMMS)        +
                   GET_X86_SIZE(MULPD_XMMF_XMMS)        +
                   GET_X86_SIZE(CVTSD2SI_RAX_XMMF)      +
                   GET_X86_SIZE(PUSH_REG);


        case SQRT:
            return GET_X86_SIZE(POP_REG)                +
                   GET_X86_SIZE(CVTSI2SD_XMMF_RAX)      +
                   GET_X86_SIZE(MOV_REG_IMMED)          +
                   sizeof(u_int64_t)                    +
                   GET_X86_SIZE(CVTSI2SD_XMMF_RAX)      +
                   GET_X86_SIZE(DIVPD_XMMF_XMMS)        +
                   GET_X86_SIZE(SQRTPD_XMM0_XMM0)       +
                   GET_X86_SIZE(MULPD_XMMF_XMMS)        +
                   GET_X86_SIZE(CVTSD2SI_RAX_XMMF)      +
                   GET_X86_SIZE(PUSH_REG);
        
        case IN:
            return GET_X86_SIZE(SUB_RSP_8)   +
                   GET_X86_SIZE(LEA_RDI_RSP) +
                   GET_X86_SIZE(PUSHA)       +
                   GET_X86_SIZE(MOV_RBP_RSP) +
                   GET_X86_SIZE(ALIGN_STACK) +
                   GET_X86_SIZE(x86_CALL)    +
                   sizeof(u_int32_t)         +
                   GET_X86_SIZE(MOV_RSP_RBP) +
                   GET_X86_SIZE(POPA);

        case OUT:
            return GET_X86_SIZE(CVTSI2SD_XMM0_RSP) +
                   GET_X86_SIZE(PUSHA)             +
                   GET_X86_SIZE(MOV_RBP_RSP)       +
                   GET_X86_SIZE(ALIGN_STACK)       +
                   GET_X86_SIZE(x86_CALL)          +
                   sizeof(u_int32_t)               +
                   GET_X86_SIZE(MOV_RSP_RBP)       +
                   GET_X86_SIZE(POPA)              +
                   GET_X86_SIZE(ADD_RSP_8);
        
        default:
            break;
    }
}

void HandleArgsCmd(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);
    size_t nativeIp = IR_HEAD->currentIp;
    size_t curIndex = IR_HEAD->currentIndex;

    log("\t #in HandleArgsCmd original cmd is %d\n",  IR_HEAD->byteCode[nativeIp]);

    int nativeNum = IR_HEAD->ir_StructArr[curIndex].nativeNum;

    if (isPushPop(nativeNum)) {

        int argType = 0;

        argType |= IR_HEAD->byteCode[nativeIp] & ARG_IMMED;
        argType |= IR_HEAD->byteCode[nativeIp] & ARG_REG;
        argType |= IR_HEAD->byteCode[nativeIp] & ARG_RAM;

        if (nativeNum == PUSH) {
            switch (argType) {

            case IMMED:     //push 10
                IR_HEAD->ir_StructArr[curIndex].nativeSize = 2;
                IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(MOV_REG_IMMED) +
                                                           sizeof(u_int64_t)           +
                                                           GET_X86_SIZE(PUSH_REG);
                break;
            
            case RAM_IMMED: //push [10]
                IR_HEAD->ir_StructArr[curIndex].nativeSize = 2;
                IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(PUSH_R15_OFFSET) +
                                                           sizeof(u_int32_t);
                                                    
                break;
            
            case REG:       //push ax
                break;

            case RAM_REG:   //push[rax]
                break;

            case REG_IMMED: //push rax+10
                break;

            case RAM_REG_IMMED: //push[rax+10]
                break;
            
            default:
                break;
            }
        }

        else {
            switch (argType) {

            case RAM_IMMED: //pop [10]
                IR_HEAD->ir_StructArr[curIndex].nativeSize = 2;
                IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(POP_R15_OFFSET) +
                                                           sizeof(u_int32_t);                                                      
                break;
            
            case REG:       //pop ax
                break;

            case RAM_REG:   //pop [rax]
                break;

            case RAM_REG_IMMED: //pop[rax+10]
                break;
            
            default:
                break;
            }

        }

        IR_HEAD->ir_StructArr[curIndex].SpecArg.argType = argType;
    }

    else if (isJump(nativeNum)) {

        IR_HEAD->ir_StructArr[curIndex].nativeSize = 2; 

        if (nativeNum == JUMP) {
            IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(x86_JMP) +
                                                       sizeof(u_int32_t);
        }
        else if (nativeNum == CALL) {
            IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(MOV_R_REG_IMMED) +
                                                       sizeof(u_int64_t)             +
                                                       GET_X86_SIZE(MOV_MEM_R14_RAX) +
                                                       GET_X86_SIZE(ADD_R14_8)       +
                                                       GET_X86_SIZE(x86_JMP)         +
                                                       sizeof(u_int32_t);
        }
        else {
            IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.jumpMask = getJumpMask(nativeNum);

            IR_HEAD->ir_StructArr[curIndex].x86_Size = GET_X86_SIZE(POP_REG)     +
                                                       GET_X86_SIZE(POP_REG)     +
                                                       GET_X86_SIZE(CMP_REG_REG) +
                                                       GET_X86_SIZE(x86_COND_JMP)+
                                                       sizeof(u_int32_t);
        }  

        size_t nextIp = IR_HEAD->byteCode[nativeIp + 1];
        IR_HEAD->ir_StructArr[curIndex].SpecArg.JumpInfo.nextIp = nextIp;
    }
}

void SetJumpAddr(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);

    log("#in SetJumpAddr\n\n");

    size_t cmdCt = IR_HEAD->nativeCmdCt;
    size_t x86RelAddr = 0;

    for (size_t index = 0; index < cmdCt; index++) {

        if (isJump(IR_HEAD->ir_StructArr[index].nativeNum)) {

            size_t nextIp = IR_HEAD->ir_StructArr[index].SpecArg.JumpInfo.nextIp;
            size_t nextRelAddr = getRelAddr(IR_HEAD, nextIp);

            int32_t diff = nextRelAddr - (x86RelAddr + IR_HEAD->ir_StructArr[index].x86_Size); // including 4 bytes of addr  

            IR_HEAD->ir_StructArr[index].SpecArg.JumpInfo.addrDiff = diff;

        }

        x86RelAddr += IR_HEAD->ir_StructArr[index].x86_Size;
    }

    IR_HEAD->x86CmdCt = x86RelAddr;
}

size_t getRelAddr(IR_HEAD_T* IR_HEAD, size_t nativeIp) {

    assert(IR_HEAD != NULL);
    log("#in getRelAddr\n\n");

    size_t cmdCt = IR_HEAD->nativeCmdCt;
    size_t RelAddr = 0;

    for (size_t index = 0; index < cmdCt; index++) {

        if (IR_HEAD->ir_StructArr[index].nativeIp == nativeIp)
            break;

        RelAddr += IR_HEAD->ir_StructArr[index].x86_Size;
    }
    
    return RelAddr;
}

void IR_CTOR(IR_HEAD_T* IR_HEAD, size_t cmdCt) {

    assert(IR_HEAD != NULL);

    log("#in IR_CTOR\n\n");

    if (cmdCt == 0)
        printf("\nWARNING: cmdCt in IR_CTOR is 0!\n\n");
    
    IR_HEAD->ir_StructArr = (ir_command*) calloc(cmdCt, sizeof(ir_command));
    assert(IR_HEAD->ir_StructArr != NULL);

    IR_HEAD->byteCode = (int*) calloc(cmdCt, sizeof(int));

    IR_HEAD->nativeCmdCt = cmdCt;

    IR_HEAD->currentIp = 0;
    IR_HEAD->currentIndex = 0;
    IR_HEAD->x86CmdCt = 0;
}

void IR_DTOR(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);

    log("\n#in IR_DTOR\n\n");

    free(IR_HEAD->ir_StructArr);
    free(IR_HEAD->byteCode);

    IR_HEAD->currentIp = 0;
    IR_HEAD->currentIndex = 0;
    IR_HEAD->x86CmdCt = 0;
}

size_t getCodeSize(FILE* byteCode) {

    assert(byteCode != NULL);

    log("#in getCodeSize\n\n");

    char a_sign[20] = "";
    fscanf(byteCode, "%2s", a_sign);
    log("signature is %s\n", a_sign);

    size_t cmd_ct = 0;
    fread(&cmd_ct, sizeof(int), 1, byteCode);
    log("cmd ct is %d\n", cmd_ct);

    return cmd_ct;
}

void readByteCode(FILE* byteCode, IR_HEAD_T* IR_HEAD) {

    assert(byteCode != NULL);
    assert(IR_HEAD != NULL);

    log("#in readByteCode\n\n");

    fread(IR_HEAD->byteCode, sizeof(int), IR_HEAD->nativeCmdCt, byteCode);

    for (int i = 0; i < IR_HEAD->nativeCmdCt; i++) {
        log("code[%2d] = %3d\n",i, IR_HEAD->byteCode[i]);
    }

}

void IR_Dump(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);

    FILE* IRdumpFile = fopen("./dotFiles/ir_dump.dot", "w");

    IR_DUMP("digraph {\n");
    IR_DUMP("rankdir=LR;\n");
    IR_DUMP("node [ shape=record ];\n");

    size_t cmdCt = IR_HEAD->nativeCmdCt;

    for (size_t i = 0; i < cmdCt; i++) {

        const char* name = IR_HEAD->ir_StructArr[i].name;

        if (isJump(IR_HEAD->ir_StructArr[i].nativeNum)) {
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <sizeP> size(byteCode): %d | <sizeN> size(native): %d |  next ip: %d\", style = \"filled\", fillcolor = \"cyan\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize, IR_HEAD->ir_StructArr[i].x86_Size ,IR_HEAD->ir_StructArr[i].SpecArg.JumpInfo.nextIp);
        }
        else if (isPushPop(IR_HEAD->ir_StructArr[i].nativeNum)) {

            char* argType = getArgType(IR_HEAD->ir_StructArr[i].SpecArg.argType); 
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <sizeP> size(byteCode): %d | <sizeN> size(native): %d | argType: %s\", style = \"filled\", fillcolor = \"green\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize, IR_HEAD->ir_StructArr[i].x86_Size, argType);
            free(argType);
        }
        else {
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <sizeP> size(byteCode) : %d | <sizeN> size(native): %d | \", style = \"filled\", fillcolor = \"gray\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize, IR_HEAD->ir_StructArr[i].x86_Size);
        }
        if (i == 0) continue;

        IR_DUMP("struct%zu -> struct%zu [weight=100];\n", i-1, i);
    }
    IR_DUMP("}\n");

    fclose(IRdumpFile);

    char cmd[100] = {0};

    sprintf(cmd, "dot -T png ./dotFiles/ir_dump.dot -o ./graphs/IR_DUMP.png");

    log("%d\n", system(cmd));

}

int isJump(int num) {
    if (num >= JUMP && num <= CALL)
        return 1;
    return 0;
}

int isPushPop(int num) {
    if (num == PUSH || num == POP)
        return 1;
    return 0;
}

u_int64_t getJumpMask(int num) {

    u_int64_t jmpMask = 0;
    switch(num) {

        case JB:
            jmpMask = JL_MASK;
        break;

        case JBE:
            jmpMask = JLE_MASK;
        break;

        case JA:    
            jmpMask = JG_MASK;
        break;

        case JAE:    
            jmpMask = JGE_MASK;
        break;

        case JE:
            jmpMask = JE_MASK;
        break;

        case JNE:
            jmpMask = JNE_MASK;
        break;

        default:
        break;
    }

    return jmpMask;
}

char* getArgType(int argType) {

    char name[15] = " ";
    switch (argType) {

        case IMMED:
            strcpy(name, "IMMED");
        break;

        case REG:
            strcpy(name, "REG");
        break;

        case REG_IMMED:
            strcpy(name, "REG_IMMED");
        break;        

        case RAM_IMMED:
            strcpy(name, "RAM_IMMED");
        break;

        case RAM_REG:
            strcpy(name, "RAM_REG");
        break;

        case RAM_REG_IMMED:
            strcpy(name, "RAM_REG_IMMED");
        break;
    }
    
    return strdup(name);
}