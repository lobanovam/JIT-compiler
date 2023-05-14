#include "../includes/language.h"
#include "../includes/parser.h"

size_t tokenId = 0;

struct node* GetGrammar(struct Token **tokens) {

    logprint("in GetGrammar\n");

    struct node* StateNode = CreateNode(SPEC_TYPE, ST, 0.0, NULL, NULL, NULL);
    struct node* nodeL = NULL;
    struct node* nodeR = NULL;

    if (curTokenType == SPEC_TYPE) {
        logprint("in GetGrammar langType is %d\n", tokens[tokenId]->value->langType);
        switch (curSpecType) {

        case IF:
            nodeL = GetIf(tokens);
            logprint("case IF\n");
            break;

        case WHILE:
            nodeL = GetIf(tokens);
            logprint("case WHILE\n");
            break;
        
        case INT_FUNC:
            logprint("case FUNC\n");
            nodeL = GetFunc(tokens);
            logprint("got FUNC\n");
            break;

        case VOID_FUNC:
            logprint("case FUNC\n");
            nodeL = GetFunc(tokens);
            logprint("got FUNC\n");
            break;

        case CALL:
            break;

        case VAR:
            logprint("case VAR\n");
            nodeL = GetVariable(tokens);
            logprint("got VAR\n");
            break;

        case RET:
            logprint("RET TIME!!\n");
            nodeL = GetRet(tokens);
            logprint("got RET\n");
            break;

        case IN:
            logprint("case IN\n");
            nodeL = GetIO(tokens, IN);
            break;

        case OUT:
            logprint("case OUT\n");
            nodeL = GetIO(tokens, OUT);
            break;

        case EQ:
            break;

        default:
            logprint("found nothing\n");
        }
    } else if (curTokenType == VAR_TYPE) {
        nodeL = GetAssign(tokens);
    }
    if (curSpecType != EOT && curSpecType != EoFOO && curSpecType != ELSE && curSpecType != ENDIF) {
        logprint("\n\ncurrent token is %d\n", tokens[tokenId]->value->langType);
        logprint("\ncontinue\n\n");
        nodeR = GetGrammar(tokens);
    } else {
        logprint("TERMINATIONAL TOKEN OR EoFOO\n");
        nodeR = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);
    }

    ConnectNodes(StateNode, nodeL, nodeR);

    logprint("out GetGrammar\n");

    return StateNode;
}

struct node* GetIO(struct Token **tokens, int IO) {
    struct node* IOnode = CreateNode(SPEC_TYPE, IO, 0.0, NULL, NULL, NULL);
    tokenId++;

    struct node* ArgNode = GetArgs(tokens);
    tokenId++;

    struct node* NILnode = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);

    ConnectNodes(IOnode, ArgNode, NILnode);

    return IOnode;
}

struct node* GetAssign(struct Token **tokens) {
    struct node* varNode = NEW_VAR_NODE(tokens, NULL, NULL);
    tokenId++;

    struct node* assignNode = NEW_SPEC_NODE(tokens, NULL, NULL);
    tokenId++;

    struct node* expNode = GetExpression(tokens);

    ConnectNodes(assignNode, varNode, expNode);

    return assignNode;
}

struct node* GetIf(struct Token **tokens) {
    struct node* ifNode = NEW_SPEC_NODE(tokens, NULL, NULL);
    tokenId++;

    ASSERT(tokens[tokenId++]->value->langType == SoCOND);
    struct node* CondNode = GetCondition(tokens);
    ASSERT(tokens[tokenId++]->value->langType == EoCOND);

    struct node* TrueNode = GetGrammar(tokens);

    if (curSpecType == ELSE) {
        struct node* ElseNode = NEW_SPEC_NODE(tokens, NULL, NULL);
        tokenId++;

        struct node* FalseNode = GetGrammar(tokens);

        ASSERT(tokens[tokenId++]->value->langType == ENDIF);

        ConnectNodes(ifNode, CondNode, ElseNode);
        ConnectNodes(ElseNode, TrueNode, FalseNode);
    } else {

        ASSERT(tokens[tokenId++]->value->langType == ENDIF);
        ConnectNodes(ifNode, CondNode, TrueNode);
    }

    return ifNode;
}


struct node* GetCondition(struct Token **tokens) {
    struct node* CondNode = NULL;
    struct node* opNode = NULL;
    CondNode = GetComp(tokens);
    
    while (curSpecType == AND || curSpecType == OR) {
        int op = curSpecType;
        tokenId++;
        struct node* CondNode2 = GetComp(tokens);
        opNode = CreateNode(SPEC_TYPE, op, 0.0, NULL, CondNode, CondNode2);

        CondNode = opNode;
    }

    return CondNode;
}

struct node* GetComp(struct Token **tokens) {
    struct node* CompNode = NULL;
    struct node* ExpNode = NULL;
    ExpNode = GetExpression(tokens);

    while (isComp(curSpecType)) {
        int comp = curSpecType;
        tokenId++;
        struct node* ExpNode2 = GetExpression(tokens);
        CompNode = CreateNode(SPEC_TYPE, comp, 0.0, NULL, ExpNode, ExpNode2);

        ExpNode = CompNode;
    }

    return ExpNode;
}

struct node* GetRet(struct Token **tokens) {
    struct node* RetNode = NEW_SPEC_NODE(tokens, NULL, NULL);
    tokenId++;
    struct node* RetValNode = GetExpression(tokens);
    struct node* NILL = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);

    ConnectNodes(RetNode, RetValNode, NILL);

    return RetNode;
}

struct node* GetFunc(struct Token **tokens) {
    logprint("in GetFunc\n");
    struct node* func = NEW_SPEC_NODE(tokens, NULL, NULL);
    int funcType = curSpecType;
    tokenId++;

    struct node* bodyFunc = NULL;
    struct node* FuncName = NEW_VAR_NODE(tokens, NULL, NULL);
    tokenId++;

    struct node* paramNode = GetGeneralParams(tokens);

    ASSERT(tokens[tokenId++]->value->langType == SoFOO);
    bodyFunc = GetGrammar(tokens);
    ASSERT(tokens[tokenId++]->value->langType == EoFOO);

    struct node* FuncTypeNode = NULL;
    if (funcType == INT_FUNC) {
        FuncTypeNode = CreateNode(SPEC_TYPE, TYPE, 0.0, NULL, NULL, NULL);
    } else {
        FuncTypeNode = CreateNode(SPEC_TYPE, VOID, 0.0, NULL, NULL, NULL);
    }

    ConnectNodes(FuncName, paramNode, FuncTypeNode);
    ConnectNodes(func, FuncName, bodyFunc);

    logprint("out GetFunc\n");
    return func;
}

struct node* GetGeneralParams(struct Token **tokens) {
    struct node* paramNode = NULL;
    struct node* paramL = NULL;
    struct node* paramR = NULL;
    if (curSpecType == EoPAR) {
        logprint("found EndOfParams\n");
        paramNode = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);
        tokenId++;
    } else {
        paramNode = CreateNode(SPEC_TYPE, PARAM, 0.0, NULL, NULL, NULL);
        paramL = GetVariable(tokens);
        paramR = GetGeneralParams(tokens); 
    }

    ConnectNodes(paramNode, paramL, paramR);
    return paramNode;
}

struct node* GetVariable(struct Token **tokens) {

    struct node* VarNode = NEW_SPEC_NODE(tokens, NULL, NULL);
    tokenId++;
    struct node* VarNameNode = NEW_VAR_NODE(tokens, NULL, NULL);
    tokenId++;
    struct node* ExpNode = NULL;

    if (curSpecType != EQ) {
        logprint("found no EQ\n");
        ExpNode = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);
    } else {
        tokenId++;
        logprint("trying to get expression\n");
        ExpNode = GetExpression(tokens);
        logprint("got expression\n");
    }

    ConnectNodes(VarNode, VarNameNode, ExpNode);
    
    return VarNode;
}

struct node* GetExpression(struct Token **tokens) {
    logprint("in GetExpression\n");

    struct node* ExpNode = NULL;

    ExpNode = GetG(tokens);

    logprint("out GetExpression\n");
    return ExpNode;
}

struct node* GetG(struct Token **tokens) {
    logprint("in GetG\n");

    struct node* newNode = NULL;
    newNode = GetE(tokens);

    logprint("out GetG\n");
    return newNode;
}

struct node* GetE(struct Token **tokens) {
    logprint("in GetE\n");

    struct node* opNode = NULL;
    struct node* nodeL = GetT(tokens);
    
    while (curSpecType == ADD || curSpecType == SUB) {
        logprint("found ADD or SUB\n");
        int op = curSpecType;
        tokenId++;
        struct node* nodeR = GetT(tokens);

        opNode = CreateNode(SPEC_TYPE, op, 0.0, NULL, nodeL, nodeR);
        nodeL = opNode;
    }

    logprint("out GetE\n");
    return nodeL;
}

struct node* GetT(struct Token **tokens) {
    logprint("in GetT\n");

    struct node* opNode = NULL;
    struct node* nodeL = GetD(tokens);
    while (curSpecType == MUL || curSpecType == DIV) {
        char op = curSpecType;
        tokenId++;
        struct node* nodeR = GetD(tokens);
        
        opNode = CreateNode(SPEC_TYPE, op, 0.0, NULL, nodeL, nodeR);

        nodeL = opNode;
    }

    logprint("out GetT\n");
    return nodeL;
}

struct node* GetD(struct Token **tokens) {
    logprint("in GetD\n");

    struct node* opNode = NULL;
    struct node* nodeL = GetP(tokens);
    while (curSpecType == POW) {
        int op = curSpecType;
        tokenId++;
        struct node* nodeR = GetP(tokens);
        opNode = CreateNode(SPEC_TYPE, op, 0.0, NULL, nodeL, nodeR);
        nodeL = opNode;
    }

    logprint("out GetD\n");
    return nodeL;
}

struct node* GetP(struct Token **tokens) {
    logprint("in GetP\n");

    struct node* node1 = NULL;
    if (curSpecType == OPEN_BR) {
        logprint("got OPEN BRACKET\n");
        tokenId++;
        node1 = GetE(tokens);
        ASSERT(curSpecType == CLOSE_BR);
        logprint("got CLOSE BRACKET\n");
        tokenId++;
    } else if (curSpecType == SQRT) {
        node1 = CreateNode(SPEC_TYPE, SQRT, 0.0, NULL, NULL, NULL);
        tokenId++;
        struct node* nodeL = GetE(tokens);
        struct node* NILnode = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);
        ConnectNodes(node1, nodeL, NILnode);
    } else {
        node1 = GetN(tokens);
    }

    logprint("out GetP\n");
    return node1;
}

struct node* GetN(struct Token **tokens) {
    logprint("in GetN\n");

    struct node* newNode = NULL;
    if (curTokenType == VAL_TYPE)
        newNode = CreateNode(VAL_TYPE, -1, tokens[tokenId]->value->dbl, NULL, NULL, NULL);
    else if (curTokenType == VAR_TYPE)
        newNode = CreateNode(VAR_TYPE, -1, 0.0, tokens[tokenId]->value->Name, NULL, NULL);
    else if (curTokenType == SPEC_TYPE) {

        if (curSpecType == CALL) {
            logprint("oh shit, this is CALL func\n");

            newNode = NEW_SPEC_NODE(tokens, NULL, NULL);                               //creating CALL
            tokenId++;

            struct node* funcName = NEW_VAR_NODE(tokens, NULL, NULL);                     //creating NAME
            tokenId++;

            struct node* ArgNode = GetArgs(tokens);
            struct node* nilNode1 = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);    //creating NIL for FUNC_NAME 

            struct node* nilNode2 = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);    //creating NIL for FUNC

            ConnectNodes(funcName, ArgNode, nilNode1);
            ConnectNodes(newNode, funcName, nilNode2);
        }
    }
    tokenId++;
    //logprint("\n\nnew node created, val is %lf\n\n", newNode->value->dbl);

    logprint("out GetN\n");
    return newNode;
}

struct node* GetArgs(struct Token **tokens) {
    struct node* ArgNode = NULL;
    struct node* ArgL = NULL;
    struct node* ArgR = NULL;
    if (curSpecType == EoPAR) {
        logprint("found EndOfParams\n");
        ArgNode = CreateNode(SPEC_TYPE, NIL, 0.0, NULL, NULL, NULL);
    
    } else {
        ArgNode = CreateNode(SPEC_TYPE, PARAM, 0.0, NULL, NULL, NULL);
        ArgL = GetExpression(tokens);
        ArgR = GetArgs(tokens); 
    }

    ConnectNodes(ArgNode, ArgL, ArgR);
    return ArgNode;
}

struct node* CreateNode(int type, int LangOp, double dbl, char *name, struct node* nodeL, struct node* nodeR) {

    struct node* newNode = (struct node*)(calloc(1, sizeof(struct node)));
    ASSERT(newNode != NULL);

    newNode->value = (union LangUnion*)calloc(1, sizeof(union LangUnion));
    ASSERT(newNode->value != NULL);

    if (type == VAL_TYPE) {
        newNode->value->dbl = dbl;
    } else if (type == VAR_TYPE) {
        newNode->value->Name = name;
    } else if (type == SPEC_TYPE) {
        newNode->value->langType = LangOp;
    } else if (type == NAME_TYPE) {
        newNode->value->Name = name; 
    }
    newNode->type = type;
    
    newNode->left  = nodeL;
    newNode->right = nodeR;
    if (nodeL)
        nodeL->parent = newNode;
    if (nodeR)
        nodeR->parent = newNode;

    return newNode;
}

void ConnectNodes(struct node* node, struct node* nodeL, struct node* nodeR) {
    node->left = nodeL;
    node->right = nodeR;

    if (nodeR) nodeR->parent = node;
    if (nodeL) nodeL->parent = node;
}

int isComp(int op) {
    if (op == IS_EE || op == IS_BE || op == IS_BT ||
        op == IS_NE || op == IS_GE || op == IS_GT)
        return 1;
    return 0;
}

