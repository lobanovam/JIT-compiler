#include "../includes/asm.h"
#include "../includes/log.h"

enum Commands
{
    #define DEF_CMD(name, num, arg, ...) \
        CMD_##name = num,

    #include "../includes/cmd.h"

    #undef DEF_CMD
};

int isEqual(const char* str1, const char* str2);

//const char* SOURCE = "./myAsm/factorial.asm";
//const char* SOURCE = "./myAsm/SquareSolver.asm";
//const char* SOURCE = "./myAsm/drawing.asm";
const char* SOURCE = "./myAsm/language.asm";

int main() {

    struct Text text = {nullptr, nullptr, 0, 0};
    struct LABEL labels[LABEL_NUM_MAX] = {{-1, ""}};

    readFile(&text);
    connectPointers(&text);

    for (int i = 0; i < text.lines_count; i++) {
        logprint("%s\n", text.pointers[i]);
    }

    //logprint("trying to calloc\n");
    int *code = (int*)(calloc(text.lines_count*2, sizeof(int)));
    ASSERT(code != nullptr);
    //logprint("calloc done\n");

    size_t ip = 0;

    SetCode(&text, code, &ip, labels);
    ip = 0;
    logprint("\n---------------------------2nd time------------------------------\n\n");
    SetCode(&text, code, &ip, labels);

    WriteCode(code, ip);

    system("cp a_code.txt ../JIT/byte_code.txt");

    return 0;
}

void skipSpace(char** ptr) {
    ASSERT(ptr != nullptr);

    while (**ptr == ' ') {
        (*ptr)++;
    }
}

void connectPointers(struct Text* text_ptr) {
    ASSERT(text_ptr != nullptr);

    logprint("    ##### in connectPointers() #####\n");
    ASSERT(text_ptr != NULL);

    *(text_ptr->pointers) = text_ptr->buff;
    for (size_t i = 1, j = 1; i < text_ptr->ch_count-1; i++) {
        if (text_ptr->buff[i] == '\n') {
            text_ptr->buff[i] = '\0';
            text_ptr->pointers[j++] = &(text_ptr->buff[i+1]);
        }
    }

    logprint("    ##### out connectPointers() #####\n");
}

void readFile(struct Text *text) {

    logprint("    ##### in readFile() #####\n\n");
    ASSERT(text != nullptr);

    FILE* source = NULL;
    if ((source = fopen(SOURCE, "rb")) == NULL) {
        printf("can't open source file\n");
    }

    fseek (source, 0L, SEEK_END);
    int buffSize = ftell (source);
    fseek (source, 0L, SEEK_SET);

    text->buff = (char*)(calloc(buffSize, sizeof(char)));

    ASSERT(text->buff != nullptr);
    logprint("    buff pointer is %p\n", text->buff);

   
    text->ch_count = fread(text->buff, sizeof(char), buffSize, source);
    fclose(source);

    countLines(text);
    logprint("    lines_count is %ld\n", text->lines_count);

    text->pointers = (char**)(calloc(text->lines_count, sizeof(char*)));
    ASSERT(text->pointers != nullptr);

    logprint("    pointers pointer is %p\n", text->pointers);
    logprint("\n    ##### out readFile() #####\n");
}

void countLines(struct Text *text) {
    logprint("\n        ##### in countLines() #####\n");
    ASSERT(text != nullptr);

    for (int i = 0; i < text->ch_count; i++) {
        if (text->buff[i] == '\n') {
            text->lines_count++;
        }
    }
    logprint("        ##### out countLines() #####\n\n");
}

int SetCode(struct Text* text, int* code, size_t *ip, struct LABEL* labels) {

    ASSERT(text != nullptr);
    ASSERT(code != nullptr);
    ASSERT(ip != nullptr);
    logprint("in setCode\n");

    size_t line = 0;
    int label_ct = 0;

    while (line < text->lines_count) {

        skipSpace(&(text->pointers[line]));

        char cmd[CMD_LEN] = "";
        sscanf(text->pointers[line], "%s", cmd);

        int len = strlen(cmd);

        if (!len) {
            line++;
            continue;
        }

        char* tmp_pt = text->pointers[line] + len;

        logprint("\ncmd was read on line %d: %s\n",line+1, cmd);

        #define DEF_CMD(name, num, arg, ...)                                            \
        if (isEqual(cmd, #name) == 0)                                                   \
        {                                                                               \
            logprint("im in %s\n", #name);                                              \
            if (arg) {                                                                  \
                skipSpace(&(tmp_pt));                                                   \
                char* curPtr = tmp_pt;                                                  \
                GetArg(text, code, ip, num, curPtr, labels);                            \
            }                                                                           \
            else {                                                                      \
                code[(*ip)++] = num;                                                    \
            }                                                                           \
            line++;                                                                     \
            continue;                                                                   \
        }

        #include "../includes/cmd.h"

        #undef DEF_CMD

        if (cmd[len-1] == ':') {
            SetLabel(labels, cmd, ip, &label_ct);
            line++;
        } else {
            logprint("undefined cmd\n");
            return 1;
        }
    }

}

int GetArg(struct Text* text, int* code, size_t* ip, int num, char* curPtr, struct LABEL* labels) {

    ASSERT(text != nullptr);
    ASSERT(code != nullptr);
    ASSERT(ip != nullptr);
    ASSERT(curPtr != nullptr);

    int scan_res = 0;
    int arg = 0;
    char reg_name[5] = "";

    logprint("curPtr is pointing to %s\n", curPtr);

    if (num >= CMD_JUMP && num <= CMD_CALL) {
        logprint("case JUMP, ");
        int label_num = 0;
        char lable_name[LABEL_NAME_LEN] = {};

        scan_res = sscanf(curPtr,"%s", lable_name);
        label_num = GetLabelNumByName(labels, lable_name);
        logprint("label_num is %d, its value is %d\n", label_num, labels[label_num].value);

        code[(*ip)++] = num;
        code[(*ip)++] = labels[label_num].value;

        return 0;
    }

    scan_res = sscanf(curPtr, "[%d+%[a-z]]%n", &arg, reg_name);

    if (scan_res != 2)
        scan_res = sscanf(curPtr, "[%[a-z]+%d]", reg_name, &arg);

    if (scan_res == 2) {
        logprint("case [d+rcx] or [rcx+d]\n");

        code[(*ip)++] = num | ARG_IMMED | ARG_REG | ARG_RAM;
        SetRegNum(code, ip, reg_name);
        code[(*ip)++] = arg;

        return 0;
    }

    scan_res = sscanf(curPtr, "%d+%[a-z]", &arg, reg_name);

    if (scan_res != 2)
        scan_res = sscanf(curPtr, "%[a-z]+%d", reg_name, &arg);

    if (scan_res == 2) {
        logprint("case d + rcx or rcx + d");

        code[(*ip)++] = num | ARG_IMMED | ARG_REG;
        SetRegNum(code, ip, reg_name);
        code[(*ip)++] = arg;

        return 0;
    }

    scan_res = sscanf(curPtr, "%[a-z]", reg_name);

    if (scan_res == 1) {
        logprint("case rcx\n");

        code[(*ip)++] = num | ARG_REG;
        SetRegNum(code, ip, reg_name);

        return 0;
    }

    scan_res = sscanf(curPtr, "%d", &arg);

    if (scan_res == 1) {
        logprint("case d\n");

        code[(*ip)++] = num | ARG_IMMED;
        code[(*ip)++] = arg;

        return 0;
    }

    scan_res = sscanf(curPtr, "[%d]", &arg);

    if (scan_res == 1) {
        logprint("case [d]\n");

        code[(*ip)++] = num | ARG_IMMED | ARG_RAM;
        code[(*ip)++] = arg;

        return 0;
    }

    scan_res = sscanf(curPtr, "[%[a-z]", reg_name);

    if (scan_res == 1) {
        logprint("case [rcx]\n");

        code[(*ip)++] = num | ARG_REG | ARG_RAM;
        SetRegNum(code, ip, reg_name);

        return 0;
    }

    printf("ERROR: UNDEFINED COMMAND\n");
    return 1;

}

int GetLabelNumByName(struct LABEL* labels, char* label_name) {
    int label_ct = 0;
    for (label_ct = 0; label_ct < LABEL_NUM_MAX; label_ct++) {
        if(strcmp(labels[label_ct].Name, label_name) == 0) {
            return label_ct;
        }
    }
    logprint("undefined label\n");
}

void WriteCode(int* code, int ip) {
    ASSERT(code != nullptr);

    FILE* a_code = NULL;

    if ((a_code = fopen(CODE, "wb")) == NULL) {
        printf("can't open code file\n");
    }
    fprintf(a_code, "%s", ASM_SIGNATURE);
    fwrite(&ip, sizeof(int), 1, a_code);
    fwrite(code, sizeof(int), ip, a_code);

    fclose(a_code);

    logprint("----------------------assemblation done----------------------\n");
}

void SetRegNum(int* code, size_t* ip, char* reg_name) {

    ASSERT(code != nullptr);
    ASSERT(ip != nullptr);
    ASSERT(reg_name != nullptr);

    if (reg_name[0] != 'r' || reg_name[2] != 'x') {
        printf("ERROR: name of register is invalid\n");
    }

    int reg_num = *(reg_name + 1) - 'a' + 1;
    logprint("reg_num: %d\n\n", reg_num);

    code[(*ip)++] = reg_num;
}

void SetLabel(struct LABEL* labels, char* cmd, size_t* ip, int* label_ct) {

    ASSERT(labels != nullptr);
    ASSERT(cmd != nullptr);
    ASSERT(ip != nullptr);
    ASSERT(label_ct != nullptr);

    logprint("case label:\n");
    strcpy(labels[*label_ct].Name, cmd);
    labels[*label_ct].value = *ip;
    logprint("now label[%d].Name = %s, .value = %d\n\n", *label_ct, labels[*label_ct].Name, labels[*label_ct].value);
    (*label_ct)++;
}

int isEqual(const char* str1, const char* str2) {

    
    while (*str1) {

        if (*str1 != *str2 && abs(*str1 - *str2) != 32) {
            break;
        }

        str1++;
        str2++;
    }

    *(const unsigned char*)str1 - *(const unsigned char*)str2;
}
