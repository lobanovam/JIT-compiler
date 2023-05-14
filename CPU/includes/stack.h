#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

//#define HASH_OFF
//#define NDEBUG
#define SAFE_MODE 1
#define DUMP_MODE 0

#ifndef NDEBUG
    #define ASSERT_OK(stk)                                                              \
        if (DUMP_MODE || StackError(stk))                                               \
        {                                                                               \
            STACK_DUMP(stk);                                                            \
            printErr(stk->errors);                                                      \
            if (SAFE_MODE && StackError(stk))                                           \
                exit(1);                                                                \
        }                                                                               \
        stk->data_hash_sum = 0;                                                         \
        stk->stack_hash_sum = 0;                                                        \
        stk->stack_hash_sum = calcGNUhash(stk, sizeof(*stk));                           \
        stk->data_hash_sum = calcGNUhash(stk->data, stk->capacity * sizeof(Elem_t));

#else
    #define ASSERT_OK(stk) ;
#endif


#define STACK_DUMP(stk)                                                         \
    logprint("%s at %s(%d)\n", __PRETTY_FUNCTION__, __FILE__, __LINE__);        \
    if (stk == NULL)                                                            \
        logprint("invalid stack pointer, can't dump\n");                        \
    logprint("Stack[%p] \"%s\" at %s at %s(%d)\n", stk, stk->stk_desc.name,     \
            stk->stk_desc.func, stk->stk_desc.file, stk->stk_desc.line);        \
    logprint("{\n");                                                            \
    logprint("  size = %zd\n", stk->ssize);                                     \
    logprint("  capacity = %zd\n", stk->capacity);                              \
    logprint("  data[%p]\n", stk->data);                                        \
    if (stk->data == NULL) {                                                    \
        logprint("invalid data pointer, cant't print stack\n");                 \
        logprint("}\n");                                                        \
    } else {                                                                    \
        logprint("  {\n");                                                      \
        printStack(stk->data, stk->capacity);                                   \
        logprint("  }\n");                                                      \
        logprint("}\n");                                                        \
    }

#define SetDescription(stk, stk_desc)                                      \
    stk_desc.line = __LINE__;                                              \
    stk_desc.name = #stk;                                                  \
    stk_desc.func = (char*)__PRETTY_FUNCTION__;                            \
    stk_desc.file = (char*)__FILE__;


#define STACK_CTOR(stk, scale)                                                               \
    SetDescription(stk, stk.stk_desc);                                                       \
    StackCtor(&stk, scale);


    //printf("calloc done with pointer %p\n", stk.data);

#ifndef HASH_OFF
    #define CHECK_HASH_SUM(stk)                                                                       \
        if(checkHash(&stk) != 0) {                                                                    \
             printf("in %s in %s(%d)\n\n",__FILE__, __PRETTY_FUNCTION__, __LINE__);                   \
             if (SAFE_MODE)                                                                           \
                exit(2);                                                                              \
        }
#else
    #define CHECK_HASH_SUM(stk) ;
#endif

#define FREE(ptr)       \
    free(ptr);          \
    ptr = nullptr;

typedef int Elem_t;
typedef unsigned long long UnsignedLL;

void StackCtor(struct Stack *stk, size_t scale);
void StackDtor(struct Stack *stk);
UnsignedLL calcGNUhash(void* start_ptr, int num_bytes);

void printStack(int *data, size_t capacity);
void printStack(double *data, size_t capacity);
void printStack(char *data, size_t capacity);

int getPoison(int);
double getPoison(double);
size_t getPoison(size_t);
char getPoison(char);
char *getPoison(char*);

Elem_t StackPop(struct Stack *stk);
void StackPush(struct Stack *stk, Elem_t value);
void StackRecalloc(struct Stack *stk, size_t ssize);

unsigned long StackError(struct Stack *stk);
void printErr(unsigned long error);

int checkHash(struct Stack *stk);

struct Description{
    char *name;
    char *func;
    int line;
    char *file;
};

struct Stack{
    UnsignedLL stack_canary_l;
    UnsignedLL* data_canary_l;
    Elem_t* data;
    UnsignedLL* data_canary_r;
    size_t ssize;
    size_t capacity;
    struct Description stk_desc;
    UnsignedLL errors;
    UnsignedLL stack_canary_r;
    UnsignedLL data_hash_sum;
    UnsignedLL stack_hash_sum;
};

static char *ErrorNames[] = {
    "data has NULL pointer",//0
    "invalid value of stack size",//1
    "invalid capacity of stack size",//2
    "size larger then capacity",//3
    "stack left canary error",//4
    "stack right canary error",//5
    "data left canary error",//6
    "data right canary error"//7
};

enum Errors {
    DATA_NULLPTR,//0
    INVALID_SIZE,//1
    INVALID_CAPACITY,//2
    SIZE_LARGER_TH_CAPACITY,//3
    STACK_CANARY_L_ERR,//4
    STACK_CANARY_R_ERR,//5
    DATA_CANARY_L_ERR,//6
    DATA_CANARY_R_ERR,//7
    AMOUNT_OF_POSSIBLE_ERRORS /*must be last*/
};

const UnsignedLL STACK_CANARY_L = 0xBAADBEEF;
const UnsignedLL STACK_CANARY_R = 0xBAADBABE;
const UnsignedLL DATA_CANARY_L = 0xDEADBEEF;
const UnsignedLL DATA_CANARY_R = 0xDEADBABE;

const UnsignedLL DEFAULT_HASH = 6969;

const size_t default_size = 5;

#endif // STACK_H_INCLUDED
