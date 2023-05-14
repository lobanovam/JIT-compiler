#ifndef DUMP_H_INCLUDED
#define DUMP_H_INCLUDED

#include <stdio.h>

#define dumpToken(str...)                                                  \
    fprintf(DumpToken, str);                                               \
    fflush(DumpToken)

#define dumpAST(str...)                                                  \
    fprintf(DumpAST, str);                                               \
    fflush(DumpAST)

#ifndef DUMP_TOKEN
    #define DUMP_TOKEN "./dotFiles/token.dot"
#endif

#ifndef DUMP_AST
    #define DUMP_AST "./dotFiles/ast.dot"
#endif

static int tokenImageNum = 1;
static int ASTImageNum = 1;

FILE *openDump(const char* name);
char* IntToString(int num);


#endif // DUMP_H_INCLUDED
