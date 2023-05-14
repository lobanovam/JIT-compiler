#include "../includes/language.h"
#include "../includes/parser.h"
#include "../includes/tokenizer.h"
#include "../includes/middleEnd.h"
#include "../includes/translator.h"

//const char* SOURCE = "../myCodes/FactorialCode.txt";
const char* SOURCE = "./myCodes/SquareSolverCode.txt";
//const char* SOURCE = "../myCodes/testCode.txt";

int main() {

    struct Text text = {nullptr, 0, 0};
    readFile(&text);
    logprint("text is \n---------------------\n%s\n---------------------\n", text.buff);
    struct Token **tokens = (struct Token**)calloc(text.ch_count, sizeof(struct Token*));
    int shit = Tokenize(tokens, &text);

    struct Tree tree = {nullptr};
    TreeCtor(&tree);

    tree.root = GetGrammar(tokens);
    AstDump(tree.root);

    int written = WriteStandartTree(tree.root);

    struct Tree ReadTree = {nullptr};
    TreeCtor(&ReadTree);

    int read = ReadStandartTree(&ReadTree);
    AstDump(ReadTree.root);

    Translate(&ReadTree);

    TreeDtor(&tree);
    TreeDtor(&ReadTree);
    return 0;
}

void TreeCtor(struct Tree* tree) {
    ASSERT(tree != nullptr);

    struct node* node1 = (struct node*)(calloc(1, sizeof(struct node)));
    node1->value  = NULL;
    node1->left   = nullptr;
    node1->right  = nullptr;
    node1->parent = nullptr;
    tree->root    = node1;
}

void NodesDtor(struct node* node) {
    ASSERT(node != nullptr);

    if (node->left) {
        NodesDtor(node->left); 
    }
    if (node->right) {
        NodesDtor(node->right);
    }

    free(node->value);
    node->value = nullptr;
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
}

void TreeDtor(struct Tree* tree) {
    ASSERT(tree != nullptr);

    NodesDtor(tree->root);
    tree->root = nullptr;
}

void readFile(struct Text *text) {


    FILE* source = NULL;
    if ((source = fopen(SOURCE, "r")) == NULL) {
        printf("can't open source file\n");
    }

    logprint("    ##### in readFile() #####\n\n");
    ASSERT(text != nullptr);

    fseek (source, 0L, SEEK_END);
    size_t buffSize = ftell (source);
    fseek (source, 0L, SEEK_SET);

    text->buff = (char*)(calloc(buffSize, sizeof(char)));

    ASSERT(text->buff != nullptr);
    logprint("    buff pointer is %p\n", text->buff);

    
    text->ch_count = fread(text->buff, sizeof(char), buffSize, source);
    fclose(source);

    trimSpaces(text);
    logprint("    lines_count is %ld\n", text->lines_count);

    logprint("\n    ##### out readFile() #####\n");
}

void trimSpaces(struct Text *text) {
    logprint("\n        ##### in trimSpaces() #####\n");
    ASSERT(text != nullptr);

    char* tmp = (char*)calloc(text->ch_count, sizeof(char));
    ASSERT(tmp != NULL);

    for (int i = 0, j = 0; i < text->ch_count; i++) {
        if (text->buff[i] == '\n') {
            text->lines_count++;
        }
        if (text->buff[i] != ' ') {
            tmp[j++] = text->buff[i];
        }
    }
    free(text->buff);
    text->buff = tmp;
    logprint("        ##### out trimSpaces() #####\n\n");
}


int Tokenize(struct Token** tokens, struct Text* text) {

    ASSERT(tokens != NULL);
    ASSERT(text != NULL);

    logprint("\nin Tokenize\n");
    size_t curLine = 1;
    size_t tokenId = 0;
    int LangOp = 0;

    for (int i = 0; i < text->ch_count; ) {
        int old_i = i;

        int curCh = text->buff[i];
        logprint("\n\ni is %d, curCH is %d, line is %zu\n", i, curCh, curLine);

        //new line
        if (curCh == '\n') {
            logprint("found new line\n");
            i++;
            curLine++;
            continue;
        }

        if (curCh == '\r') {
            logprint("found \\r \n");
            i++;
            continue;
        }

        //trying to get reprise (while)
        else if (strncmp(text->buff+i, CMD_WHILE.name, CMD_WHILE.len) == 0) {

            i+=CMD_WHILE.len;
            logprint("found WHILE\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, WHILE, 0.0, NULL, curLine);
            ASSERT(text->buff[i++] == '"');
            tokens[tokenId++] = CreateToken(SPEC_TYPE, SoCOND, 0.0, NULL, curLine);

            continue;
        }

        //trying to get learn notes (in)
        else if (strncmp(text->buff+i, CMD_IN.name, CMD_IN.len) == 0) {

            logprint("found IN\n");
            i+=CMD_IN.len;

            tokens[tokenId++] = CreateToken(SPEC_TYPE, IN, 0.0, NULL, curLine);
            ASSERT(text->buff[i++] == '[');

            continue;
        }

        //trying to get play (out)
        else if (strncmp(text->buff+i, CMD_OUT.name, CMD_OUT.len) == 0) {

            logprint("found OUT\n");
            i+=CMD_OUT.len;

            tokens[tokenId++] = CreateToken(SPEC_TYPE, OUT, 0.0, NULL, curLine);
            ASSERT(text->buff[i++] == '[');

            continue;
        }

        //trying to get coda (ret)    
        else if (strncmp(text->buff+i, CMD_RET.name, CMD_RET.len) == 0) {

            logprint("found coda\n");
            i+=CMD_RET.len;

            tokens[tokenId++] = CreateToken(SPEC_TYPE, RET, 0.0, NULL, curLine);

            continue;
        } 
        
        //trying to get if (conductor says)
        else if (strncmp(text->buff+i, CMD_IF.name, CMD_IF.len) == 0) {

            i+=CMD_IF.len;
            logprint("found IF");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, IF, 0.0, NULL, curLine);
            ASSERT(text->buff[i++] == '"');
            tokens[tokenId++] = CreateToken(SPEC_TYPE, SoCOND, 0.0, NULL, curLine);

            continue;
        }

        //trying to get ELSE (disagree)
        else if (strncmp(text->buff+i, CMD_ELSE.name, CMD_ELSE.len) == 0) {

            i+=CMD_ELSE.len;
            logprint("found ELSE\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, ELSE, 0.0, NULL, curLine);

            continue;
        }

        //trying to get ENDIF (done)
        else if (strncmp(text->buff+i, CMD_ENDIF.name, CMD_ENDIF.len) == 0) {

            i+=CMD_ENDIF.len;
            logprint("found ENDIF\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, ENDIF, 0.0, NULL, curLine);

            continue;
        }

        //start of function
        else if (curCh == '{') {
            logprint("found {, start of function\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, SoFOO, 0.0, NULL, curLine);
            i++;
        }

        //end of function
        else if (curCh == '}') {
            logprint("found }, end of function\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, EoFOO, 0.0, NULL, curLine);
            i++;
        }

        //end of params
        else if (curCh == ']') {
            tokens[tokenId++] = CreateToken(SPEC_TYPE, EoPAR, 0.0, NULL, curLine);
            logprint("found ), end of params\n");
            i++;
        }

        //coma between parans
        else if (curCh == ',') {
            i++;
        }

        //trying to get neg
        else if (!strncmp(text->buff+i, CMD_NEG.name, CMD_NEG.len)) {
            i+=CMD_NEG.len;
            tokens[tokenId++] = CreateToken(VAL_TYPE, -1, -1, NULL, curLine);

            continue;
        }

        //trying to get sqrt
        else if (!strncmp(text->buff+i, CMD_SQRT.name, CMD_SQRT.len)) {
            i+=CMD_SQRT.len;
            tokens[tokenId++] = CreateToken(SPEC_TYPE, SQRT, -1, NULL, curLine);

            continue;
        }

        //trying to get Value
        else if (isdigit(curCh)) {
            logprint("found val\n");
            int digits = 0;

            double val = getVal(text->buff+i, &digits);

            if (!digits) {
                logprint("SYNTAX ERROR ON LINE %zd\n", curLine);
                return 1;
            }

            i += digits;

            tokens[tokenId++] = CreateToken(VAL_TYPE, -1, val, NULL, curLine);
            continue;
        }
        //trying to get bassKey (void func)
         else if (strncmp(text->buff+i, CMD_VOID_FUNC.name, CMD_VOID_FUNC.len) == 0) {

            logprint("found bassKey\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, VOID_FUNC, 0.0, NULL, curLine);
            i+=CMD_VOID_FUNC.len;

            logprint("cur ch is %c\n", text->buff[i]);

            char* funcName = getFuncName(text->buff+i);
            if (!funcName) {
                logprint("SYNTAX ERROR ON LINE %zu\n", curLine);
                return 1;
             }
            i+=strlen(funcName);

            tokens[tokenId++] = CreateToken(NAME_TYPE, 0, 0.0, funcName, curLine);

            logprint("after func ch is %c\n", text->buff[i]);
            ASSERT(text->buff[i++] == '[');

            continue;
        }

        //trying to get trebleKey (int func)
        else if (strncmp(text->buff+i, CMD_INT_FUNC.name, CMD_INT_FUNC.len) == 0) {

            logprint("found trebleKey\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, INT_FUNC, 0.0, NULL, curLine);
            i+=CMD_INT_FUNC.len;

            logprint("cur ch is %c\n", text->buff[i]);

            char* funcName = getFuncName(text->buff+i);
            if (!funcName) {
                logprint("SYNTAX ERROR ON LINE %zu\n", curLine);
                return 1;
             }
            i+=strlen(funcName);

            tokens[tokenId++] = CreateToken(NAME_TYPE, 0, 0.0, funcName, curLine);

            logprint("after func ch is %c\n", text->buff[i]);
            ASSERT(text->buff[i++] == '[');

            continue;
        }
        
        //trying to get tatto
        else if (strncmp(text->buff+i, CMD_TACT.name, CMD_TACT.len) == 0) {
            logprint("found tatto\n");
            i+=CMD_TACT.len;

            continue;
        }

        //trying to get varName
        else if (curCh >= 'a' && curCh <='g') {
            char* varName = getVarName(text->buff+i);
        
            tokens[tokenId++] = CreateToken(VAR_TYPE, 0.0, 0.0, varName, curLine);
            i += strlen(varName);

            continue;
        }

        //trying to get 2 sign
        else if ((LangOp = getOp2(text->buff[i], text->buff[i+1])) != 0) {
            logprint("got 2 signed operator\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, LangOp, 0.0, NULL, curLine);
            i+=2;

            continue;
        }

        //trying to get 1 sign 
        else if ((LangOp = getOp1(curCh)) != 0) {
            logprint("got 1 signed operator\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, LangOp, 0.0, NULL, curLine);
            i++;

            continue;
        }

        //trying to get EoCOND
        else if (curCh == '"') {
            logprint("found end of condition\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, EoCOND, 0.0, NULL, curLine);
            i++;

            continue;
        }

        //trying to get note
        else if (strncmp(text->buff+i, CMD_VAR.name , CMD_VAR.len) == 0) {
            logprint("found note\n");
            logprint("cur ch is %c\n", text->buff[i]);
                
            tokens[tokenId++] = CreateToken(SPEC_TYPE, VAR, 0.0, NULL, curLine);
            i+=CMD_VAR.len;

            char* variable = getVarName(text->buff+i);
            tokens[tokenId++] = CreateToken(VAR_TYPE, 0.0, 0.0, variable, curLine);
            i+=strlen(variable);

            continue;
        }

        //trying to get CALL (D.S.alcoda)
        else if (strncmp(text->buff+i, CMD_CALL.name, CMD_CALL.len) == 0) {
            logprint("found CALL func\n");
            tokens[tokenId++] = CreateToken(SPEC_TYPE, CALL, 0.0, NULL, NULL);
            i+=CMD_CALL.len;
                        
            logprint("cur ch is %c\n", text->buff[i]);

            char* funcName = getFuncName(text->buff+i);
            if (!funcName) {
                logprint("SYNTAX ERROR ON LINE %zu\n", curLine);
                return 1;
            }
            i+=strlen(funcName);

            tokens[tokenId++] = CreateToken(NAME_TYPE, 0, 0.0, funcName, curLine);
            logprint("got call func %s\n", funcName);
            ASSERT(text->buff[i++] == '[');

            continue;
        }

        //End Of Tokens
        if (old_i == i) {
            tokens[tokenId++] = CreateToken(SPEC_TYPE, EOT, 0.0, NULL, curLine);
            logprint("trying to tokenDump\n");
            tokenDump(tokens, tokenId);
            logprint("tokenDump done\n");
            logprint("END OF IMPUT ON LINE %zu\n", curLine);

            return 1;
        }
    }
    
    return 0;
}

double getVal(const char* str, int* digits) {

    double val = 0;
    int i = 0;
    while ('0' <= str[i] && str[i] <= '9') {
            val = val * 10 + str[i] - '0';
            i++;
        }
    logprint("res val is %lf\n", val);

    *digits = i;
    return val;
}

char* getVarName(const char* str) {
    logprint("in getVarName cur ch is %c\n", *str);

    char* varName = (char*)calloc(MAX_VAR_NAME, sizeof(char));
    ASSERT(varName != NULL);

    int i = 0;
    while (str[i] >= 'a' && str[i] <= 'g') {
        varName[i] = str[i];
        if (str[i+1] == '#' || str[i+1] == '?') {
            varName[i+1] = str[i+1];
            i++;
        }
        i++;
    }
    if (!i) varName = NULL;
    logprint("result var name is %s, its len is %zu\n", varName, strlen(varName));

    return varName;
}

char* getFuncName(const char* str) {
    logprint("in getFuncName cur ch is %c\n", *str);

    char* funcName = (char*)calloc(MAX_FUNC_NAME, sizeof(char));
    ASSERT(funcName != NULL);

    int i = 0;
    while (str[i] != '(' && isalpha(str[i])) {
        funcName[i] = str[i];
        i++;
    }
    if (!i) funcName = NULL;
    logprint("result func name is %s, its len is %zu\n", funcName, strlen(funcName));

    return funcName;
}

struct Token* CreateToken(int type, int LangOp, double dbl, char* name, size_t line) {

    struct Token* newToken = (struct Token*)calloc(1, sizeof(struct Token));

    newToken->value = (union LangUnion*)calloc(1, sizeof(union LangUnion));
    ASSERT(newToken->value != NULL);

    newToken->type = type;
    if (type == VAL_TYPE) {
       newToken->value->dbl = dbl;
    } else if (type == VAR_TYPE) {
        newToken->value->Name = name;
    } else if (type == SPEC_TYPE) {
        newToken->value->langType = LangOp;
    } else if (type == NAME_TYPE) {
        newToken->value->Name = name; 
    }
    newToken->line = line;

    return newToken;
}

void tokenDump(struct Token **tokens, size_t tokenSize) {

    ASSERT(tokens != NULL);

    FILE* DumpToken = openDump(DUMP_TOKEN);

    dumpToken("digraph {\n");
    dumpToken("rankdir=LR;\n");
    dumpToken("node [ shape=record ];\n");

    for (int i = 0; i < tokenSize; i++) {

        switch(tokens[i]->type) {
            case VAL_TYPE:
                dumpToken("struct%d [\nlabel = \"<type>type: VAL_TYPE|<dbl>dbl: %lf\", style = \"filled\", fillcolor = \"grey\" \n];\n", i, tokens[i]->value->dbl);
            break;

            case VAR_TYPE:
                dumpToken("struct%d [\nlabel = \"<type>type: VAR_TYPE|<var>var: %s\", style = \"filled\", fillcolor = \"green\" \n];\n", i, tokens[i]->value->Name);
            break;

            case NAME_TYPE:
                 dumpToken("struct%d [\nlabel = \"<type>type: NAME_TYPE|<name>name: %s\", style = \"filled\", fillcolor = \"cyan\" \n];\n", i, tokens[i]->value->Name);
            break;

            case SPEC_TYPE:
                char color[10] = " ";
                if (tokens[i]->value->langType == EoFOO || tokens[i]->value->langType == SoFOO) {
                    strcpy(color, "yellow");
                } else if (tokens[i]->value->langType == INT_FUNC || tokens[i]->value->langType == VOID_FUNC){
                    strcpy(color, "red");
                } else if (tokens[i]->value->langType == IF    ||
                           tokens[i]->value->langType == ELSE  ||
                           tokens[i]->value->langType == ENDIF) {
                    strcpy(color, "orange");
                } else if (tokens[i]->value->langType == EOT){
                    strcpy(color, "purple");
                } else {
                    strcpy(color, "blue");
                }
                char* name = getSpecName(tokens[i]->value->langType);
                dumpToken("struct%d [\nlabel = \"<type>type: SPEC_TYPE|<spec>spec: %s\", style = \"filled\", fillcolor = \"%s\" \n];\n", i, name, color);
            break;
        }

        if (i == 0) continue;

        dumpToken("struct%d -> struct%d [weight=100];\n", i-1, i);
    }
    dumpToken("}\n");

    fclose(DumpToken);

    char cmd[100] = {0};

    sprintf(cmd, "dot -T png ./dotFiles/token.dot -o ./graphs/graphTokenCode%d.png", tokenImageNum);

    logprint("%d\n", system(cmd));

    tokenImageNum++;
}

void AstDump(struct node* node) {
    ASSERT(node != nullptr);
    logprint("in ast dump\n");

    FILE* DumpAST = openDump(DUMP_AST);

    dumpAST("digraph {\n");
    dumpAST("graph [dpi = 100]");
    dumpAST("rankdir = TB;\n");
    dumpAST("node [ shape=record ];\n");

    DrawTree(node, DumpAST);

    dumpAST("}");

    fclose(DumpAST);

    char cmd[100] = {0};

    sprintf(cmd, "dot -T png ./dotFiles/ast.dot -o ./graphs/graphAST%d.png", ASTImageNum);

    logprint("%d\n", system(cmd));

    ASTImageNum++;
}

void DrawTree(struct node* node,  FILE* DumpAST) {

    ASSERT(node != nullptr);
    ASSERT(DumpAST != nullptr);

    if (node->type == SPEC_TYPE) {
        char *spec = getSpecName(node->value->langType);
        dumpAST("struct%p [\nlabel = \"{Spec: %s |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"lightgrey\" \n];\n", node, spec, node->parent, node->left, node->right);
    } else if (node->type == VAL_TYPE) {
        dumpAST("struct%p [\nlabel = \"{value: %lf |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"green\" \n];\n", node, node->value->dbl, node->parent, node->left, node->right);
    } else if (node->type == VAR_TYPE) {
        dumpAST("struct%p [\nlabel = \"{var: %s |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"cyan\" \n];\n", node, node->value->Name, node->parent, node->left, node->right);
    } else if (node->type == NAME_TYPE) {
        dumpAST("struct%p [\nlabel = \"{name: %s |parent: %p| left: %p| right: %p }\", style = \"filled\", color = \"black\", fillcolor = \"cyan\" \n];\n", node, node->value->Name, node->parent, node->left, node->right);
    }

    if (node->parent != NULL) {
        dumpAST("struct%p -> struct%p [weight=900 constraint=true color=red];\n", node->parent, node);
    }
    if (node->left != NULL) {
        DrawTree(node->left, DumpAST);
    }
    if (node->right != NULL) {
        DrawTree(node->right, DumpAST);
    }
}

char* getSpecName(int spec) {
    char name[15] = " ";
    switch (spec) {

    case IN:
        strcpy(name, "IN");
        break;

    case OUT:
        strcpy(name, "OUT");
        break;

    case OPEN_BR:
        strcpy(name, "(");
        break;

    case CLOSE_BR:
        strcpy(name, ")");
        break;

    case ST:
        strcpy(name, "ST");
        break;

    case EOT:
        strcpy(name, "EOT");
        break;

    case ADD:
        strcpy(name, "ADD");
        break;
    
    case SUB:
        strcpy(name, "SUB");
        break;

    case MUL:
        strcpy(name, "MUL");
        break;
    
    case DIV:
        strcpy(name, "DIV");
        break;

    case POW:
        strcpy(name, "POW");
        break;

    case SQRT:
        strcpy(name, "SQRT");
        break;

    case AND:
        strcpy(name, "AND");
        break;

    case OR:
        strcpy(name, "OR");
        break;

    case EQ:
        strcpy(name, "EQ");
        break;

    case IS_EE:
        strcpy(name, "IS_EE");
        break;

    case IS_BE:
        strcpy(name, "IS_BE");
        break;

    case IS_GE:
        strcpy(name, "IS_GE");
        break;

    case IS_GT:
        strcpy(name, "IS_GT");
        break;

    case IS_BT:
        strcpy(name, "IS_BT");
        break;

    case IS_NE:
        strcpy(name, "IS_NE");
        break;

    case IF:
        strcpy(name, "IF");
        break;

    case ELSE:
        strcpy(name, "ELSE");
        break;

    case NIL:
        strcpy(name, "NIL");
        break;

    case VAR:
        strcpy(name, "VAR");
        break;

    case WHILE:
        strcpy(name, "WHILE");
        break;

    case INT_FUNC:
        strcpy(name, "FUNC");
        break;

    case VOID_FUNC:
        strcpy(name, "FUNC");
        break;

    case FUNC:
        strcpy(name, "FUNC");
        break;

    case TYPE:
        strcpy(name, "TYPE");
        break;

    case VOID:
        strcpy(name, "VOID");
        break;

    case RET:
        strcpy(name, "RET");
        break;

    case CALL:
        strcpy(name, "CALL");
        break;

    case PARAM:
        strcpy(name, "PARAM");
        break;
    
    case SoFOO:
        strcpy(name, "SoF");
        break;
    
    case EoFOO:
        strcpy(name, "EoF");
        break;

    case SoCOND:
        strcpy(name, "SoC");
        break;

    case EoCOND:
        strcpy(name, "EoC");
        break;

    case ENDIF:
        strcpy(name, "ENDIF");
        break;

    case EoPAR:
        strcpy(name, "EoPAR");
        break;
    }

    return strdup(name);
}

int getOp1(int ch) {
    switch (ch) {
    case '+':
        return ADD;
        break;
    
    case '-':
        return SUB;
        break;

    case '/':
        return DIV;
        break;

    case '*':
        return MUL;
        break;

    case '^':
        return POW;
        break;

    case '=':
        return EQ;
        break;

    case '>':
        return IS_GT;
        break;

    case '<':
        return IS_BT;
        break;

    case '(':
        return OPEN_BR;
        break;

    case ')':
        return CLOSE_BR;
        break;

    default:
        return 0;
    }
}

int getOp2(int ch1, int ch2) {
    if (ch2 == '=') {
        switch (ch1) {
        case '=':
            return IS_EE;
            break;
        
        case '>':
            return IS_GE;
            break;

        case '<':
            return IS_BE;
            break;

         case '!':
            return IS_NE;
            break;
        default:
            return 0;
        
        }
    } else if (ch2 == '|' && ch1 == '|') {
        return OR;
    } else if (ch2 == '&' && ch1 == '&') {
        return AND;
    } else return 0;
}
