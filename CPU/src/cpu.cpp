#include "../includes/cpu.h"
#include "../includes/log.h"

int main() {

    logprint("----------------------cpu started ----------------------\n");
    struct Stack lastCall = {};
    STACK_CTOR(lastCall, default_size);
    logprint("lastCall created\n");

    struct Stack stk1 = {};
    STACK_CTOR(stk1, default_size);
    logprint("stk1 created\n");

    struct CPU cpu = {NULL, NULL, NULL, CPU_SIGNATURE, 0, lastCall, stk1};

    size_t cmd_ct = getCode(&cpu);

    if (Compile(&cpu, cmd_ct)) {
        printf("ERROR OCCURED\n");
    }

    printf("ok, im done here\n");

    CpuDtor(&cpu);

    return 0;
}

int Compile(struct CPU* cpu, size_t cmd_ct) {

    ASSERT(cpu != NULL);

    while (cpu->ip < cmd_ct) {
        switch(cpu->code[cpu->ip] & CMD_MASK) {
            
            #define DEF_CMD(name, num, arg, ...) \
                case num:                        \
                __VA_ARGS__                      \
                break;

            #include "../includes/cmd.h"

            #undef DEF_CMD

        default:
            printf("ERROR: undefined command, ip: %zd\n", cpu->ip);
            return UNDEFINED_CMD;
        }
        cpu->ip++;
    }

    return 0;
}

void clearBuff() {
    char ch = 0;
    while((ch = getchar()) != '\n')
        continue;
}

void PushArg(struct CPU* cpu) {

    logprint("\nin push, ");
    size_t cmd_ip = cpu->ip++;

    if (cpu->code[cmd_ip] & ARG_RAM) {

        int index = 0;
        if (cpu->code[cmd_ip] & ARG_REG) {
            index += cpu->regs[cpu->code[(cpu->ip)++]];
        }

        if (cpu->code[cmd_ip] & ARG_IMMED) {
            index += cpu->code[cpu->ip++];
        }
        logprint("trying to push %d, ", cpu->RAM[index]);
        StackPush(&(cpu->stk1), cpu->RAM[index]);

    } else {

        double arg   = 0;
        if (cpu->code[cmd_ip] & ARG_REG) {
            arg += (double) cpu->regs[cpu->code[(cpu->ip)++]] / PRECISION;
        }

        if (cpu->code[cmd_ip] & ARG_IMMED) {
            arg += (double) cpu->code[cpu->ip++];
        }
        logprint("trying to push %.3lf, ", arg);
        StackPush(&(cpu->stk1), (int)(arg * PRECISION));
    }

    cpu->ip--;
    logprint("push done\n");
}

void PopArg(struct CPU* cpu) {

    logprint("\nin pop, ");
    int arg = StackPop(&(cpu->stk1));
    logprint("trying to pop %d, ", arg);
    size_t cmd_ip = cpu->ip++;

    int index = 0;

    if (cpu->code[cmd_ip] & ARG_RAM) {

        if (cpu->code[cmd_ip] & ARG_REG) {
            index += cpu->regs[cpu->code[(cpu->ip)++]] / PRECISION;
        }

        if (cpu->code[cmd_ip] & ARG_IMMED) {
            index += cpu->code[cpu->ip++];
        }

        cpu->RAM[index] = arg;

    } else {

        if (cpu->code[cmd_ip] & ARG_REG) {
            cpu->regs[cpu->code[(cpu->ip)++]] = arg;
        }

        if (cpu->code[cmd_ip] & ARG_IMMED) {
            printf("SYNTAX ERROR: Invalid usage of function pop");
        }
    }
    cpu->ip--;
    logprint("pop done\n");

}

void CpuCtor(struct CPU* cpu, size_t cmd_ct) {

    cpu->code = (int*)(calloc(cmd_ct, sizeof(int)));
    ASSERT(cpu->code != nullptr);

    static int ram_c[RAM_SIZE] = {0};
    cpu->RAM = ram_c;

    static int reg_c[REG_SIZE] = {0};
    cpu->regs = reg_c;
}

void CpuDtor(struct CPU* cpu) {

    ASSERT(cpu != nullptr);

    FREE(cpu->code);
    cpu->RAM = nullptr;
    cpu->regs = nullptr;
    StackDtor(&(cpu->lastCall));
    StackDtor(&(cpu->stk1));
}

int checkSign(struct CPU* cpu, FILE* a_code) {

    ASSERT(cpu != nullptr);
    ASSERT(a_code != nullptr);

    char a_sign[20] = "";
    fscanf(a_code, "%2s", a_sign);

    if (strcmp(a_sign, cpu->signature)) {
        return WRONG_SIGNATURE;
    }
    return 0;
}

size_t getCode(struct CPU* cpu) {

    ASSERT(cpu != nullptr);

    FILE* a_code = NULL;
    if ((a_code = fopen(CODE, "rb")) == NULL) {
        printf("can't open a_code file\n");
    }
    ASSERT(a_code != NULL);

    if (checkSign(cpu, a_code))
        return WRONG_SIGNATURE;

    size_t cmd_ct = 0;

    fread(&cmd_ct, sizeof(int), 1, a_code);
    logprint("cmd ct is %d\n", cmd_ct);

    CpuCtor(cpu, cmd_ct);

    fread(cpu->code, sizeof(int), cmd_ct, a_code);

    for (int i = 0; i < cmd_ct; i++) {
        printf("code[%2d] = %3d\n",i, cpu->code[i]);
    }
    fclose(a_code);

    return cmd_ct;
}

void PrintRAM(int* RAM) {

    size_t screen_size = RAM_SIZE + RAM_SIZE/LEN_LINE + 1;
    char* screen = (char*)(calloc(screen_size, sizeof(char)));
    for (int i = 0, j = 0; i < RAM_SIZE; i++, j++) {
        if (i % LEN_LINE == 0) {
            screen[j] = '\n';
            j++;
        }
        if (RAM[i])  screen[j] = '*';
        else screen[j] = '.';
    }
    fwrite(screen, sizeof(char), screen_size, stdout);
    printf("\n");
}


