#include "../includes/language.h"
#include "../includes/middleEnd.h"

int WriteStandartTree(struct node* node) {
    FILE* Standart = fopen("AstTree.txt", "w");
    if (Standart == NULL) {
        printf("can't open AstTree file in writer\n");
        return 1;
    }

    WriteNode(node, Standart, 0);

    fclose(Standart);

    return 0;
}

int WriteNode(struct node* node, FILE* Standart, int spaces) {
    //logprint("started Writing Node\n");

    ASSERT(node != NULL);
    ASSERT(Standart != NULL);

    int hasChild = 0;

    printSpaces(spaces, Standart);

    fprintf(Standart, "{ ");

     switch (node->type) {
        case VAL_TYPE:
            PutVal(Standart, node->value->dbl);
            break;

        case VAR_TYPE:
            PutVar(Standart, node->value->Name);
            break;

        case SPEC_TYPE:
            PutSpec(Standart, node->value->langType);
            hasChild = 1;
            break;

        case NAME_TYPE:
            PutVar(Standart, node->value->Name);
            fprintf(Standart, "\n");
            hasChild = 1;
            break;

        default:
            printf("TYPE ERROR: Unexpected node type");
    }

    if (node->left) {
        WriteNode(node->left, Standart, spaces + 4);
    }
    if (node->right) {
        WriteNode(node->right, Standart, spaces + 4);
    }

    if (hasChild && (node->left || node->left)) printSpaces(spaces, Standart);

    if (hasChild && (node->left || node->left)) fprintf(Standart, "}\n");
    else fprintf(Standart, " }\n");

    return 0;
}

void printSpaces(int spaceCt, FILE* Standart) {
    ASSERT(Standart != nullptr);

    while (spaceCt-- > 0) {
        fprintf(Standart, " ");
    }
}

void PutVal(FILE* Standart, double dbl) {
    fprintf(Standart, "%.1lf", dbl);
}

void PutVar(FILE* Standart, char* name) {
    fprintf(Standart, "\"%s\"", name);
}

void PutSpec(FILE* Standart, int langOp) {
    char* SpecName = getSpecName(langOp);
    fprintf(Standart, "%s", SpecName);
    if (langOp != NIL && langOp != TYPE &&
        langOp != VOID)
        fprintf(Standart, "\n");
}

int ReadStandartTree(struct Tree* tree) {
    FILE* Standart = fopen("AstTree.txt", "r");
    if (Standart == NULL) {
        printf("can't open AstTree file in reader\n");
        return 1;
    }

    tree->root = ReadTree(NULL, Standart, LEFT);

    return 0;
}

struct node* ReadTree(struct node* parent, FILE* Standart, int side) {
    logprint("start reading tree\n");

    ASSERT(Standart != NULL);

    struct node* node = NULL;
    char str[50] = "";
    int ch_count = 0;
    int hasChild = 0;

    fscanf(Standart, "{ %s", str);

    logprint("1st scan is %s\n", str);

    fscanf(Standart, " }%n ", &ch_count);

    if (!ch_count) {
        hasChild = 1;
        logprint("has child\n");
    }

    int LangOp = GetOp(str);
    logprint("got langop %d\n", LangOp);

    if (LangOp) {
        node = CreateNode(SPEC_TYPE, LangOp, 0.0, NULL, NULL, NULL);
        logprint("spec node created\n");
    } else {
        //log("start writing var or val\n");
        double dbl = 0;

        logprint("ch_count before val = %d\n", ch_count);
        ch_count = 0;
        sscanf(str, "%lf%n", &dbl, &ch_count);
        logprint("ch_count in val = %d\n", ch_count);

        if (ch_count) {
            node = CreateNode(VAL_TYPE, -1, dbl, NULL, NULL, NULL);
        } else {
            char* var = (char*)calloc(20, sizeof(char));

            sscanf(str, " \"%[a-zA-Z_#?]\" %n", var, &ch_count);
            logprint("var is %s\n", var);

            if (!ch_count)
                logprint("ERROR: line: %d var: %s\n", str, var);
            
            if (parent && parent->type == SPEC_TYPE && 
               (parent->value->langType == FUNC || parent->value->langType == CALL))
                node = CreateNode(NAME_TYPE,  -1, 0.0, var, NULL, NULL);
            else node = CreateNode(VAR_TYPE, -1, 0.0, var, NULL, NULL);
        }

    }

    ASSERT(node != nullptr);

    node->parent = parent;


    if      (parent && side == LEFT)  node->parent->left = node;
    else if (parent && side == RIGHT) node->parent->right = node;

    if (!hasChild) return node;

    node->left  = ReadTree(node, Standart, LEFT);
    node->right = ReadTree(node, Standart, RIGHT);

    fscanf(Standart, " }%n ", &ch_count);


    return node;

}

int GetOp(char* str) {
    if (!strcmp(str, "FUNC"))
        return FUNC;

    else if (!strcmp(str, "IN"))
        return IN;

    else if (!strcmp(str, "OUT"))
        return OUT;

    else if (!strcmp(str, "ST"))
        return ST;

    else if (!strcmp(str, "ADD"))
        return ADD;

    else if (!strcmp(str, "SUB"))
        return SUB;

    else if (!strcmp(str, "DIV"))
        return DIV;

    else if (!strcmp(str, "MUL"))
        return MUL;

    else if (!strcmp(str, "SQRT"))
        return SQRT;

    else if (!strcmp(str, "POW"))
        return POW;

    else if (!strcmp(str, "AND"))
        return AND;

    else if (!strcmp(str, "OR"))
        return OR;

    else if (!strcmp(str, "EQ"))
        return EQ;

    else if (!strcmp(str, "IS_EE"))
        return IS_EE;

    else if (!strcmp(str, "IS_GE"))
        return IS_GE;

    else if (!strcmp(str, "IS_BE"))
        return IS_BE;
    
    else if (!strcmp(str, "IS_GT"))
        return IS_GT;

    else if (!strcmp(str, "IS_BT"))
        return IS_BT;

    else if (!strcmp(str, "IS_NE"))
        return IS_NE;

    else if (!strcmp(str, "ST"))
        return ST;

    else if (!strcmp(str, "IF"))
        return IF;

    else if (!strcmp(str, "ELSE"))
        return ELSE;

    else if (!strcmp(str, "NIL"))
        return NIL;

    else if (!strcmp(str, "VAR"))
        return VAR;

    else if (!strcmp(str, "WHILE"))
        return WHILE;
    
    else if (!strcmp(str, "TYPE"))
        return TYPE;

    else if (!strcmp(str, "VOID"))
        return VOID;

    else if (!strcmp(str, "RET"))
        return RET;
    
    else if (!strcmp(str, "CALL"))
        return CALL;

    else if (!strcmp(str, "PARAM"))
        return PARAM;

    return UNKNOWN;

}