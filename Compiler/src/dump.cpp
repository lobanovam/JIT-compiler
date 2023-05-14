#include <stdio.h>
#include <stdlib.h>

#include "../includes/dump.h"

FILE *openDump(const char* name) {
    FILE *Dump = fopen(name, "w");
    if (Dump == NULL)
        printf("ERROR, CAN'T OPEN DUMP FILE\n");
    return Dump;
}

char* IntToString(int num) {
    char* str = (char*)(calloc(10, sizeof(char)));
    int tmp = num;
    int decimal = 1;
    int digits =  0;
    while (tmp > 0) {
        tmp/=10;
        digits++;
        decimal*=10;
    }
    decimal/=10;
    for (int i = 0; i < digits; i++) {
        str[i] = num / decimal + '0';
        num = num%decimal;
        decimal/=10;
    }
    return str;
}