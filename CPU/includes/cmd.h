
DEF_CMD(HLT, 0, 0,
{
    cpu->ip = 0;
    logprint("in hlt\n");
    return 0;
})

DEF_CMD(PUSH, 1, 1,
{
   PushArg(cpu);
})

DEF_CMD(POP, 2, 1,
{
    PopArg(cpu);
})

DEF_CMD(ADD, 3, 0,
{
    logprint("in add, ");
    double a = (double)StackPop(&(cpu->stk1)) / PRECISION;
    double b = (double)StackPop(&(cpu->stk1)) / PRECISION;
    logprint("trying to add %.3lf + %.3lf\n", a, b);
    double res = (a + b) * PRECISION;
    StackPush(&(cpu->stk1), (int)res);
})

DEF_CMD(SUB, 4, 0,
{
    logprint("in sub, ");
    double a = (double)StackPop(&(cpu->stk1)) / PRECISION;
    double b = (double)StackPop(&(cpu->stk1)) / PRECISION;
    logprint("trying to sub %.3lf - %.3lf\n", a, b);
    double res = (a - b) * PRECISION;
    StackPush(&(cpu->stk1), (int)res);
})

DEF_CMD(MUL, 5, 0,
{
    logprint("in mul, ");
    double a = (double)StackPop(&(cpu->stk1)) / PRECISION;
    double b = (double)StackPop(&(cpu->stk1)) / PRECISION;
    logprint("trying to mul %.3lf * %.3lf\n", a, b);
    double res = a * b * PRECISION;
    StackPush(&(cpu->stk1), (int)res);
})

DEF_CMD(DIV, 6, 0,
{
    logprint("in div, ");
    double a = (double)StackPop(&(cpu->stk1)) / PRECISION;
    int b = StackPop(&(cpu->stk1));
    if (b == 0) {
        printf("ERROR: can't divide by zero\n");
        return DEV_BY_0;
    } else {
        double c = (double)b / PRECISION;
        logprint("trying to div %.3lf / %.3lf\n", a, c);
        double res = (a/c) * PRECISION;
        StackPush(&(cpu->stk1), (int)res);
    }
})

DEF_CMD(IN, 7, 0,
{
    logprint("in IN, ");
    printf("\nenter a number\n");
    int value = 0;

    while (!scanf("%d", &value)) {
        printf("please, enter a number\n");
        clearBuff();
    }
    clearBuff();
    StackPush(&(cpu->stk1), value * PRECISION);

})

DEF_CMD(OUT, 8, 0,
{
    //printf("--------------------------------------\n");
    double a = (double)StackPop(&(cpu->stk1)) / PRECISION;
    printf("OUT PRINT IS %.3lf\n", a);
    //printf("--------------------------------------\n");
})

DEF_CMD(JUMP, 9, 1,
{
    logprint("\nin jump\n");
    cpu->ip = cpu->code[cpu->ip+1]-1;
})

DEF_CMD(JB, 10, 1,
{
    logprint("in JB, ");
    if (StackPop(&(cpu->stk1)) < StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else{
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(JBE, 11, 1,
{
    logprint("in JBE, ");
    if (StackPop(&(cpu->stk1)) <= StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else {
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(JA, 12, 1,
{
    logprint("in JA, ");
    if (StackPop(&(cpu->stk1)) > StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else {
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(JAE, 13, 1,
{
    logprint("in JAE, ");
    if (StackPop(&(cpu->stk1)) >= StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else {
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(JE, 14, 1,
{
    logprint("in JE, ");
    if (StackPop(&(cpu->stk1)) == StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else {
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(JNE, 15, 1,
{
    logprint("in JNE, ");
    if (StackPop(&(cpu->stk1)) != StackPop(&(cpu->stk1))){
        cpu->ip = cpu->code[cpu->ip+1]-1;
        logprint("condition is true\n");
    } else{
        cpu->ip++;
        logprint("condition is false\n");
    }
})

DEF_CMD(CALL, 16, 1,
{
    logprint("in call\n");
    StackPush(&(cpu->lastCall), cpu->ip);
    cpu->ip = cpu->code[cpu->ip+1]-1;
})

DEF_CMD(RET, 17, 0,
{
    logprint("trying to RET, ");
    cpu->ip = StackPop(&(cpu->lastCall))+1;
    logprint("ret done\n");
})

DEF_CMD(SQRT, 18, 0,
{
    logprint("in sqrt, ");
    int a = StackPop(&(cpu->stk1));
    logprint("trying to sqrt(%.3lf)\n", (double)a / PRECISION);
    if (a < 0) {
        printf("ERROR: can't sqrt of negative number\n");
        return NEG_SQRT;
    }
    double b = sqrt((double) a / PRECISION);
    StackPush(&(cpu->stk1), (int)(b * PRECISION));
})

DEF_CMD(COPY, 19, 0,
{
    logprint("\nin copy, ");
    int value = StackPop(&(cpu->stk1));
    logprint("trying to copy %d, ", value);
    StackPush(&(cpu->stk1), value);
    StackPush(&(cpu->stk1), value);
    logprint("copy done\n");
})

DEF_CMD(RAM_PRINT, 20, 0,
{
    logprint("trying to print RAM\n");
    PrintRAM(cpu->RAM);
})

