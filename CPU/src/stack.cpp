#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../includes/stack.h"
#include "../includes/log.h"

void StackRecalloc(struct Stack *stk, size_t ssize){

    char* tmp = (char*)stk->data;
    tmp -= sizeof(UnsignedLL);

    tmp = (char*)(realloc((char*)tmp, ssize*sizeof(Elem_t) + 2*sizeof(UnsignedLL)));
    stk->data_canary_l = (UnsignedLL*) tmp;
    stk->data_canary_r = (UnsignedLL*) (tmp + sizeof(UnsignedLL) + ssize * sizeof(Elem_t));

    *(stk->data_canary_l)= DATA_CANARY_L;
    *(stk->data_canary_r) = DATA_CANARY_R;

    stk->data = (Elem_t*)(tmp + sizeof(UnsignedLL));

    if (ssize > stk->capacity) {
        for (size_t i = 0; i < ssize - stk->capacity; i++) {
            stk->data[stk->ssize + i] = getPoison(stk->data[stk->ssize + i]);
        }
    }
    stk->capacity = ssize;
}

void StackCtor(struct Stack *stk, size_t scale) {
    stk->stack_canary_l = STACK_CANARY_L;
    stk->stack_canary_r = STACK_CANARY_R;

    stk->capacity = scale;
    stk->errors = 0;
    stk->ssize = 0;

    size_t enlarged_size = scale * sizeof(Elem_t) + 2 * sizeof(UnsignedLL);
    char* tmp  = (char*)(calloc(enlarged_size, sizeof(char)));
    stk->data_canary_l = (UnsignedLL*)tmp;
    stk->data_canary_r = (UnsignedLL*)(tmp + sizeof(UnsignedLL) + scale*sizeof(Elem_t));
    stk->data = (Elem_t*)(tmp + sizeof(UnsignedLL));

    *(stk->data_canary_l)= DATA_CANARY_L;
    *(stk->data_canary_r) = DATA_CANARY_R;

    ASSERT_OK(stk);
}

void StackDtor(struct Stack *stk){
    ASSERT_OK(stk);

    stk->capacity = 0;
    stk->ssize = 0;
    char* tmp = (char*)stk->data;
    tmp -= sizeof(UnsignedLL);
    free(tmp);
}

void StackPush(struct Stack *stk, Elem_t value) {
    //CHECK_HASH_SUM(*stk);
    ASSERT_OK(stk);

    if (stk->ssize == stk->capacity) {
        StackRecalloc(stk, stk->capacity*2);
    }
    stk->data[stk->ssize++] = value;
    ASSERT_OK(stk);
}

Elem_t StackPop(struct Stack *stk) {
    ASSERT_OK(stk);

    if (stk->ssize <= stk->capacity/4) {
        StackRecalloc(stk, stk->capacity/2);
    }

    Elem_t tmp = stk->data[--stk->ssize];
    stk->data[stk->ssize] = getPoison(stk->data[stk->ssize]);

    ASSERT_OK(stk);

    return tmp;
}

void printStack(int *data, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %d\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }
}

void printStack(double *data, size_t capacity){
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %f\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }
}

void printStack(char *data, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            if (data[i] != 0) {
                logprint("    *[%zd] = %c\n", i, data[i]);
            } else {
                logprint("    *[%zd] = %d\n", i, data[i]);
            }
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }
}

void printStack(size_t *data, size_t capacity) {
    for (size_t i = 0; i < capacity; i++) {
        if (data[i] != getPoison(data[i])){
            logprint("    *[%zd] = %zu\n", i, data[i]);
        } else {
            logprint("    [%zd] = %x (POISON)\n", i, data[i]);
        }
    }


}

unsigned long StackError(struct Stack *stk) {

    if (stk->data == NULL)
            stk->errors |= (1 << DATA_NULLPTR);

    if (stk->ssize > stk->capacity)
            stk->errors |= (1 << SIZE_LARGER_TH_CAPACITY);

    if ((int)stk->ssize < 0)
            stk->errors |= (1 << INVALID_SIZE);

    if ((int)stk->capacity < 0)
            stk->errors |= (1 << INVALID_CAPACITY);

    if (stk->stack_canary_l != STACK_CANARY_L)
            stk->errors |= (1 << STACK_CANARY_L_ERR);

    if (stk->stack_canary_r != STACK_CANARY_R)
            stk->errors |= (1 << STACK_CANARY_R_ERR);

    if (*(stk->data_canary_l) != DATA_CANARY_L)
            stk->errors |= (1 << DATA_CANARY_L_ERR);

    if (*(stk->data_canary_r) != DATA_CANARY_R)
            stk->errors |= (1 << DATA_CANARY_R_ERR);

    return(stk->errors);
}

void printErr(unsigned long error) {
    for (size_t i = 0; i < AMOUNT_OF_POSSIBLE_ERRORS; i++) {
        if (error & (1 << i))
            printf("ERROR: %s\n", ErrorNames[i]);
    }
}

int getPoison(int) {
    return 0xCAFEBABE;
}

size_t getPoison(size_t) {
    return 0xCAFEBABE;
}

double getPoison(double) {
    return NAN;
}

char getPoison(char) {
    return 'z' + 1;
}

char* getPoison(char*) {
    return (char*)0xCAFEBABE;
}

UnsignedLL calcGNUhash(void* start_ptr, int num_bytes) {
    UnsignedLL hash_sum = DEFAULT_HASH;
    for (size_t i = 0; i < num_bytes; i++) {
        hash_sum = hash_sum * 33 + ((char*)start_ptr)[i];
    }

    return hash_sum;
}

int checkHash(struct Stack *stk) {
    int status = 0;

    UnsignedLL tmp_data_hash =  stk->data_hash_sum; /*was updated by previous ASSERT_OK*/
    UnsignedLL tmp_stack_hash =  stk->stack_hash_sum; /*was updated by previous ASSERT_OK*/

    stk->data_hash_sum = 0;
    stk->stack_hash_sum = 0;

    UnsignedLL n_stack_hash_sum = calcGNUhash(stk, sizeof(*stk));
    UnsignedLL n_data_hash_sum = calcGNUhash(stk->data, stk->capacity * sizeof(Elem_t));

    if (tmp_data_hash != n_data_hash_sum) {
        stk->data_hash_sum = n_data_hash_sum;
        printf("DATA_HASH_SUM WAS MODIFIED ");
        status++;
    } else {
        stk->data_hash_sum = tmp_data_hash;
    }

    if (tmp_stack_hash != n_stack_hash_sum) {
        stk->stack_hash_sum = n_stack_hash_sum;
        printf("STACK_HASH_SUM WAS MODIFIED\n");
        status++;
    } else {
        stk->stack_hash_sum = tmp_stack_hash;
    }

    return status;
}






