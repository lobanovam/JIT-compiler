#include <assert.h>
#include <string.h>

#include <sys/mman.h>
#include <time.h>

#include "../includes/log.h"
#include "../includes/jit.h"
#include "../includes/translator.h"


const char* BYTE_CODE_FILE = "./byte_code.txt";

void ExecuteJIT(X86_CODE_T* X86_CODE);
void mprotect_change_rights (X86_CODE_T* X86_CODE, int protect_status);

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
    log("x86CmdCt is %zu\n", IR_HEAD.x86CmdCt);
    X86_CODE_CTOR(&X86_CODE, IR_HEAD.x86CmdCt);

    Translate(&IR_HEAD, &X86_CODE);

    ExecuteJIT(&X86_CODE);
    
}

void ExecuteJIT(X86_CODE_T* X86_CODE) {

    assert(X86_CODE != NULL);

    log("\n#in ExecuteJIT\n\n");

    int mprotect_status = mprotect (X86_CODE->BinaryCode, X86_CODE->totalSize, PROT_EXEC | PROT_READ | PROT_WRITE);
    
    if (mprotect_status == -1)
        return;

    mprotect_change_rights (X86_CODE, PROT_EXEC | PROT_READ | PROT_WRITE);

    void (* OneMoreSegFault)(void) = (void (*)(void))(X86_CODE->BinaryCode);
    assert (OneMoreSegFault != nullptr);

    log("\nSTARTING!!!\n\n");

    clock_t start = clock();
    for (int i = 0; i < 10000; i++) {
        OneMoreSegFault();
    }
    clock_t end = clock();

    printf("\ntime is %lf\n", (double)(end - start) * 1000 / (double) CLOCKS_PER_SEC);

    
    log("\nDONE!!!\n\n");

    mprotect_change_rights (X86_CODE, PROT_READ | PROT_WRITE);

}

void mprotect_change_rights (X86_CODE_T* X86_CODE, int protect_status)
{
    int mprotect_status = mprotect (X86_CODE->BinaryCode, X86_CODE->totalSize + 1, protect_status);
    if (mprotect_status == -1)
    {
        printf ("Mrotect error\n");
        return;
    }
}