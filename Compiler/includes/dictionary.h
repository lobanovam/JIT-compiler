#ifndef DICT_H_INCLUDED
#define DICT_H_INCLUDED

struct CMD {
    const char* name;
    int len;
};

const struct CMD CMD_IF = {"conductorsays", 13};

const struct CMD CMD_VAR = {"note", 4};

const struct CMD CMD_RET = {"coda", 4};

const struct CMD CMD_INT_FUNC = {"trebleKey", 9};

const struct CMD CMD_VOID_FUNC = {"bassKey", 7};

const struct CMD CMD_TACT = {"tatto", 5};

const struct CMD CMD_CALL = {"D.S.alcoda", 10};

const struct CMD CMD_NEWLINE = {"\n", 1};

const struct CMD CMD_ELSE = {"disagree", 8};

const struct CMD CMD_ENDIF = {"done", 4};

const struct CMD CMD_IN = {"learnnotes", 10};

const struct CMD CMD_OUT = {"play", 4};

const struct CMD CMD_WHILE = {"reprise", 7};

const struct CMD CMD_NEG = {"neg", 3};

const struct CMD CMD_SQRT = {"sqrt", 4};

#endif




