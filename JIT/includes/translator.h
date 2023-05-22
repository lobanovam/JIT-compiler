#include <stdio.h>
#include <stdlib.h>

#define GET_X86_SIZE(name)  SIZE_##name 

typedef struct X86_CODE_T {

    char* BinaryCode;
    char* memSegment;
    u_int64_t* CallStackSegment;

    size_t totalSize;
    size_t curLen; 

} X86_CODE_T ;

enum X86_CMD : u_int64_t {   

    //mov r_x, num
    MOV_REG_IMMED = 0xb848,           // need to "|" with shifted by 8 reg mask
                                      // must be followed with 64 bit integer 
    //mov r_x, r_x
    MOV_REG_REG = 0xc08948,           // first: need to "|" with shited left by 16 reg mask, second - by 19

    //mov r_x, [r15 + offset]
    MOV_REG_R15_OFFSET = 0x498b87,    // need to "|" with shited left by 19 reg mask
                                      // must be followed with 32 bit offset of r15
    //mov [r15 + offset], r_x
    MOV_R15_OFFSET_REG = 0x878949,    // the same as above

    //mov qword [r14], rax
    MOV_MEM_R14_RAX = 0x068949,

    PUSH_REG = 0x50,                  // "|" regMask

    //push [r15 + offset]
    PUSH_R15_OFFSET = 0xb7ff41,       // must be followed by 32bit offset

    POP_REG = 0x58,                    // "|" regMask

    //pop [r15 + offset]
    POP_R15_OFFSET = 0x878f41,         // must be followed by 32bit offset

    ARITHM_REG_REG = 0xc00048,          // first reg - "|" with shifted by 16 REG_MASK, second reg - "|" with shifted by 19 REG_MASK
                                        // ADD / SUB = "|" with shifted by 8 ARITM_MASK

    IMUL_REG_REG = 0xc0af0f48,         // first reg - "|" with shifted by 27 REG_MASK, second reg - "|" with shifted by 24 REG_MASK

    CQO = 0x9948,                      // signed rax -> (rdx, rax)

    IDIV_REG = 0xf8f748,               //  "|" with shifted by 16 REG_MASK

    CVTSI2SD_XMMF_RAX = 0xc02a0f48f2,   // mov xmmf, rax    xmmf "|" with shifted by 35 (32 + 3)
    CVTSD2SI_RAX_XMMF = 0xc02d0f48f2,   // mov rax, xmm0    xmmf "|" with shifted by 32 (32)
    
    CVTSI2SD_XMM0_RSP = 0x24042a0ff2,     // mov xmm0, [rsp]
    CVTSI2SD_XMM1_RSP_8 = 0x08244c2a0ff2, // mov xmm1, [rsp+8]

    ADD_RSP_16 = 0x10c48348,
    SUB_RSP_8  = 0x08ec8348,
    ADD_RSP_8  = 0x08c48348,

    ADD_R14_8 = 0x08c68349,     // for callStack
    SUB_R14_8 = 0x08ee8349,

    // push [r14]
    PUSH_MEM_R14 = 0x36ff41, 

    DIVPD_XMMF_XMMS = 0xc05e0f66,       // xmmF/xmmS -> xmmF     xmmF "|" with shifted by 27 (24 + 3), xmmS - by 24

    MULPD_XMMF_XMMS = 0xc0590f66,       // xmm0*xmm1 -> xmm0     same 

    SQRTPD_XMM0_XMM0 = 0xc0510f66,      // sqrt(xmm0) -> xmm0 

    x86_RET = 0xC3,                     // ret

    x86_JMP = 0xe9,                     // followed by 32-bit difference (dst - src)

    x86_COND_JMP = 0x000f,              // apply cond_jmp mask shifted by 8

    x86_CALL = 0xe8,

    CMP_REG_REG =  0xc03948,            // shifted by 19 and by 16 REG_MASK

    ALIGN_STACK = 0xf0e48348,
    MOV_RBP_RSP = 0xe48949,
    MOV_RSP_RBP = 0xe4894c,
    LEA_RDI_RSP = 0x00247c8d48,

    PUSHA = 0x505152535241,  
    POPA  = 0x5a415b5a5958,

    MOV_R_REG_IMMED = 0xb849, // "|" with shifted by 8 r_reg mask, followed with 64bit abs ptr of memory
    PUSH_R_REG = 0x5041,      // "|" with shifted by 8 r_reg mask


};



enum x86_Commands_Size {

    SIZE_MOV_MEM_R14_RAX = 3,
    SIZE_ADD_R14_8 = 4,
    SIZE_SUB_R14_8 = 4,
    SIZE_PUSH_MEM_R14 = 3,

    SIZE_PUSH_R_REG = 2,
    SIZE_MOV_R_REG_IMMED = 2, 

    SIZE_PUSHA = 6,
    SIZE_POPA = 6,

    SIZE_ALIGN_STACK = 4,
    SIZE_MOV_RBP_RSP = 3,
    SIZE_MOV_RSP_RBP = 3,
    SIZE_LEA_RDI_RSP = 5,

    SIZE_MOV_REG_IMMED = 2,
    SIZE_MOV_REG_REG = 3,
    SIZE_MOV_REG_R15_OFFSET = 3,
    SIZE_MOV_R15_OFFSET_REG = 3,
    SIZE_PUSH_REG = 1,
    SIZE_PUSH_R15_OFFSET = 3,
    SIZE_POP_REG = 1,
    SIZE_POP_R15_OFFSET = 3,

    SIZE_ARITHM_REG_REG = 3,

    SIZE_IMUL_REG_REG = 4,
    SIZE_CQO = 2,
    SIZE_IDIV_REG = 3,

    SIZE_CVTSI2SD_XMMF_RAX = 5,
    SIZE_CVTSD2SI_RAX_XMMF = 5,

    SIZE_CVTSI2SD_XMM0_RSP = 5,
    SIZE_CVTSI2SD_XMM1_RSP_8 = 6,

    SIZE_ADD_RSP_16 = 4,
    SIZE_SUB_RSP_8  = 4,
    SIZE_ADD_RSP_8  = 4,

    SIZE_SQRTPD_XMM0_XMM0 = 4,
    SIZE_DIVPD_XMMF_XMMS = 4,
    SIZE_MULPD_XMMF_XMMS = 4,

    SIZE_x86_RET = 1,
    SIZE_x86_JMP = 1,
    SIZE_x86_COND_JMP = 2,
    SIZE_x86_CALL = 1,
    SIZE_CMP_REG_REG = 3,
};

enum XMM_MASK : u_int64_t {
    XMM0,
    XMM1,
    XMM2,
    XMM3, 
};

enum REG_MASK : u_int64_t {
    RAX,
    RCX,
    RDX,
    RBX,
    RSP,
    RBP,
    RSI,
    RDI,
};

enum R_REGS : u_int64_t {
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
};

enum ARITHM_MASK : u_int64_t {

    ADD_MASK = 0x01,
    SUB_MASK = 0x29,

};

enum COND_JMPS : u_int64_t {
    JE_MASK = 0x84,
    JNE_MASK = 0x85,
    JG_MASK = 0x8f,
    JGE_MASK = 0x8d,
    JL_MASK = 0x8c,
    JLE_MASK = 0x8e,
};



void X86_CODE_CTOR(X86_CODE_T* X86_CODE, size_t totalSize);
void X86_CODE_DTOR(X86_CODE_T* X86_CODE);

void Translate(IR_HEAD_T* IR_HEAD, X86_CODE_T* X86_CODE);

