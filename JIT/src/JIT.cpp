#include <assert.h>
#include <string.h>

#include "../includes/log.h"
#include "../includes/jit.h"

const char* BYTE_CODE_FILE = "byte_code.txt";

int main() {

    FILE* byteCode = fopen(BYTE_CODE_FILE, "rb");
    assert(byteCode != NULL);

    size_t ByteCodeSize = getCodeSize(byteCode);

    IR_HEAD_T IR_HEAD = {};
    IR_CTOR(&IR_HEAD, ByteCodeSize);

    readByteCode(byteCode, &IR_HEAD);

    SetIR(&IR_HEAD);

    IR_Dump(&IR_HEAD);

}

void SetIR(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);

    log("#in SetIR\n\n");

    size_t CmdCt = IR_HEAD->nativeCmdCt;

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

    IR_HEAD->ir_StructArr[curIndex].name = name;
    IR_HEAD->ir_StructArr[curIndex].hasArgs = hasArgs;
    IR_HEAD->ir_StructArr[curIndex].nativeIp = nativeIp;
    IR_HEAD->ir_StructArr[curIndex].nativeNum = IR_HEAD->byteCode[IR_HEAD->currentIp] & CMD_MASK;

    if (hasArgs) {
        
        log("\t %s has args\n", name);
        HandleArgsCmd(IR_HEAD);
    }
    else {
        log("\t %s has no args\n", name);
        IR_HEAD->ir_StructArr[curIndex].nativeSize = 1;
        IR_HEAD->ir_StructArr[curIndex].x86_Size = 10;     // ????????????????????
    }

    
    IR_HEAD->currentIp += IR_HEAD->ir_StructArr[curIndex].nativeSize;
    IR_HEAD->currentIndex++;
}

void HandleArgsCmd(IR_HEAD_T* IR_HEAD) {

    assert(IR_HEAD != NULL);
    size_t nativeIp = IR_HEAD->currentIp;
    size_t curIndex = IR_HEAD->currentIndex;

    log("\t #in HandleArgsCmd original cmd is %d\n",  IR_HEAD->byteCode[nativeIp]);

    int nativeNum = IR_HEAD->ir_StructArr[curIndex].nativeNum;

    if (nativeNum == PUSH || nativeNum == POP) {

        //log("found push or pop\n");

        int argType = 0;

        argType |= IR_HEAD->byteCode[nativeIp] & ARG_IMMED;
        argType |= IR_HEAD->byteCode[nativeIp] & ARG_REG;
        argType |= IR_HEAD->byteCode[nativeIp] & ARG_RAM;

        //log("argType is %d, original cmd is %d\n", argType, IR_HEAD->byteCode[nativeIp]);

        if (argType == IMMED || argType == REG || argType == RAM_IMMED || argType == RAM_REG) {

            log("\t \t size is 2\n");
            IR_HEAD->ir_StructArr[curIndex].nativeSize = 2;
            IR_HEAD->ir_StructArr[curIndex].x86_Size = 10; //???????????????????????????????

        }

        else if (argType == REG_IMMED || argType == RAM_REG_IMMED) {
            
            log("\t \t size is 3\n");
            IR_HEAD->ir_StructArr[curIndex].nativeSize = 3;
            IR_HEAD->ir_StructArr[curIndex].x86_Size = 10; //???????????????????????????????

        }
        IR_HEAD->ir_StructArr[curIndex].SPECIAL_ARG = argType;
    }

    else if (nativeNum >= JUMP && nativeNum <= CALL ) {

        log("\t \t found some sort of jump or call\n");

        IR_HEAD->ir_StructArr[curIndex].nativeSize = 2;   // ????????????????
        IR_HEAD->ir_StructArr[curIndex].x86_Size = 10;    // ????????????????

        size_t nextIp = IR_HEAD->byteCode[nativeIp + 1];
        IR_HEAD->ir_StructArr[curIndex].SPECIAL_ARG = nextIp;
    }
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

}

size_t getCodeSize(FILE* byteCode) {

    assert(byteCode != NULL);

    log("#in getCodeSize\n\n");

    char a_sign[20] = "";
    fscanf(byteCode, "%2s", a_sign);
    log("signature is %s\b", a_sign);

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
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <size> size(native): %d | next ip: %d\", style = \"filled\", fillcolor = \"cyan\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize,  IR_HEAD->ir_StructArr[i].SPECIAL_ARG);
        }
        else if (isPushPop(IR_HEAD->ir_StructArr[i].nativeNum)) {

            char* argType = getArgType(IR_HEAD->ir_StructArr[i].SPECIAL_ARG); 
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <size> size(native): %d | argType: %s\", style = \"filled\", fillcolor = \"green\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize, argType);
        }
        else {
            IR_DUMP("struct%zu [\nlabel = \"<index> index: %zu|<name>name: %s|<nativeIp>ip: %zu | <size> size(native): %d \", style = \"filled\", fillcolor = \"gray\" \n];\n", i, i, name, IR_HEAD->ir_StructArr[i].nativeIp, IR_HEAD->ir_StructArr[i].nativeSize);
        }
        if (i == 0) continue;

        IR_DUMP("struct%d -> struct%d [weight=100];\n", i-1, i);
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