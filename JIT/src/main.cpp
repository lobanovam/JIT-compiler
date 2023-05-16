#include <assert.h>
#include <string.h>

#include "../includes/log.h"
#include "../includes/jit.h"
#include "../includes/translator.h"


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

    SetJumpAddr(&IR_HEAD);

    X86_CODE_T X86_CODE = {};
    X86_CODE_CTOR(&X86_CODE, IR_HEAD.x86CmdCt);

    
}