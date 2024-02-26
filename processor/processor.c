#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include "../stack/stack.h"
#include "../assembler/asm.h"
#include "../onegin/fileReader.h"
#include "processor.h"

#define SIZE_OF_RAM 256

#define DEBUG

#ifdef DEBUG
#define ERRLOG(arg, ...)       \
    fileLog(__VA_ARGS__);      \
    return arg
#else
#define ERRLOG(arg, ...)    return arg
#endif

#define ARITHM(arg)                                                                     \
    do {                                                                                \
        if (stackPush(stackName, temp1 arg temp2))                                      \
        {                                                                               \
            ERRLOG(ARITHM_GRP_ERR, msgZeroArgs, "ARITHM_GRP_ERR", "", 0, "[error]");    \
            stackPrint(stackName, IN_CONSOLE);                               \
        }                                                                               \
    } while(0)  

#define COMPARE(arg)                \
    do                              \
    {                               \
        if (temp2 arg temp1)        \
        {                           \
            jmp(bytecode);          \
            return 0;               \
        }                           \
    } while (0)

    


const char *msgZeroArgs = "%-25s| %-17s| %-20d| %-20s|\n";
const char *msgOneArgD = "%-25s| %-17d| %-20d| %-20s|\n";
const char *msgOneArgX = "%-25s| 0x%-15.2X| %-20d| %-20s|\n";

typedef struct
{
    stack Stack;
    stack returnPtrs;
    char RAM[SIZE_OF_RAM];
    dataType Register[REGISTERS_NUM];
} processor_t;

typedef struct
{
    char *bytecode;
    int bytecodeSize;
    int ip;
} bytecode_t;

FILE *logFile = NULL;

int arithmetics(stack *stackName, const int lowerMask);
void jmp(bytecode_t *bytecode);
int caseJMP(stack *stackName, bytecode_t *bytecode, const int cmd);
int caseMov(processor_t *SPU, bytecode_t *bytecode);
int graphics(processor_t *SPU, const char * bytecode, int *ip);
void fileLog(const char *format, ...);
char *getCmd(const bytecode_t *bytecode, const signed int skipNum);
int ipInc(bytecode_t *bytecode, const unsigned int incNum);
void fileLog(const char *format, ...);
int doCommand(processor_t *processor, bytecode_t *bytecode);

int main(int argc, char** argv)
{
    assert(argc >= 2);
    bytecode_t bytecode = {0};

    fileRead(argv[1], &bytecode.bytecode, NULL, &bytecode.bytecodeSize, NULL, BUFF_ONLY);

    char *log1 = NULL;
    char *log2 = NULL;

    if (argc >= 3 && argv[2] != NULL)
        log1 = argv[2];
    else
        log1 = "DataStack.log";

    if (argc >= 4 && argv[3] != NULL)
        log2 = argv[3];
    else
        log2 = "ReturnStack.log";

#ifdef DEBUG
    if (argc >= 5 && argv[4] != NULL)
        logFile = fopen(argv[4], "w");
    else
        logFile = fopen("processorErrorLog.log", "w");    
#endif

    processor_t processor = {0};
    stackCtor(&processor.Stack, 10, log1);
    stackCtor(&processor.returnPtrs, 5, log2);

    int lessSignBitMask = 0;
    int errorNum = 0;
    for (; bytecode.ip < bytecode.bytecodeSize;)
        if (errorNum = doCommand(&processor, &bytecode))
            break;
            
        
    stackDtor(&processor.Stack);
    stackDtor(&processor.returnPtrs);
    free(bytecode.bytecode);
    fclose(logFile);

    return errorNum;
}

int arithmetics(stack *stackName, const int command)
{
    assert(stackName != NULL);
    dataType temp1 = 0;
    dataType temp2 = 0;

    if (stackPop(stackName, &temp1) || stackPop(stackName, &temp2))
        return -1;
       
    switch (command)
    {
    case ADD:
        ARITHM(+);
        break;
    case SUB:
        ARITHM(-);
        break;
    case MULT:
        ARITHM(*);
        break;
    case DIV:
        ARITHM(/);
        break;
    default:
        ERRLOG(FATAL_ERR, msgZeroArgs, "FATAL_ERROR", "ARITHM_GRP", 0, "[error]");
        break;
    }

    return 0;
}

void jmp(bytecode_t *bytecode)
{
    assert(bytecode != NULL);
    bytecode->ip = *(int *)(getCmd(bytecode, 0));
}

int caseJMP(stack *stackName, bytecode_t *bytecode, const int cmd)
{
    assert(stackName != NULL);
    assert(bytecode != NULL);
    int temp1 = 0;
    int temp2 = 0;

    if (stackPop(stackName, &temp2) || stackPop(stackName, &temp1))
        return 1;

    if (stackPush(stackName, temp1))
        {ERRLOG(RPUSH_CMD_FAIL, msgZeroArgs, "RPUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
    stackPush(stackName, temp2);

    switch(cmd)
    {
    case JMP:
        jmp(bytecode);
        break;
    case JB:
        COMPARE(<);
        break;
    case JBE:
        COMPARE(<=);
        break;
    case JA:
        COMPARE(>);
        break;
    case JAE:
        COMPARE(>=);
        break;
    case JE:
        COMPARE(==);
        break;
    case JNE:
        COMPARE(!=);
        break;
    default:
        ERRLOG(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        break;    
    }
    ipInc(bytecode, sizeof(int));
}

int caseMov(processor_t *SPU, bytecode_t *bytecode)
{
    switch (bytecode->bytecode[bytecode->ip - 1] & LESS_SIGN_MASK)
    {

    case REG_FRST | REG_SCND:
        SPU->Register[*getCmd(bytecode, 0) - 1] = SPU->Register[*getCmd(bytecode, sizeof(char)) - 1];
        ipInc(bytecode, sizeof(char) + sizeof(char));
        break;

    case REG_FRST | RAM_SCND:
        SPU->Register[*getCmd(bytecode, 0) - 1] = *(dataType *)(SPU->RAM + *(int *)getCmd(bytecode, sizeof(char)));
        ipInc(bytecode, sizeof(char) + sizeof(int));
        break;

    case REG_FRST | NUM_SCND:
        SPU->Register[*getCmd(bytecode, 0) - 1] = *(dataType *)(getCmd(bytecode, sizeof(char)));
        ipInc(bytecode, sizeof(char) + sizeof(dataType));
        break;

    case RAM_FRST | REG_FRST:
        *(dataType *)(SPU->RAM + *(int *)(getCmd(bytecode, 0))) =  SPU->Register[*getCmd(bytecode, sizeof(int)) - 1];
        ipInc(bytecode, sizeof(int) + sizeof(char));
        break;

    case RAM_FRST | RAM_SCND:
        *(dataType *)(SPU->RAM + *(int *)(getCmd(bytecode, 0))) = *(dataType *)(SPU->RAM + *(int *)(getCmd(bytecode, sizeof(int))));
        ipInc(bytecode, sizeof(int) + sizeof(int));
        break;

    case RAM_FRST | NUM_SCND:
        *(dataType *)(SPU->RAM + *(int *)(getCmd(bytecode, 0))) = *(dataType *)(getCmd(bytecode, sizeof(int)));
        ipInc(bytecode, sizeof(int) + sizeof(dataType));
        break;

    default:
        ERRLOG(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        break;
    }

    return 0;
}

int doCommand(processor_t *processor, bytecode_t *bytecode)
{
    assert(processor != NULL);
    assert(bytecode != NULL);
    char cmd = *getCmd(bytecode, 0);
    ipInc(bytecode, sizeof(char));

    int lessSignBitMask = cmd & LESS_SIGN_MASK;
    int dataHolder = 0;

    switch (cmd & MOST_SIGN_MASK)
    {
    case PUSH_GRP:
        switch (cmd)
        {
        case PUSH:
            if (stackPush(&processor->Stack, *(dataType *)(getCmd(bytecode, 0))))
                {ERRLOG(PUSH_CMD_FAIL, msgZeroArgs, "PUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
            ipInc(bytecode, sizeof(dataType));
            break;

        case RPUSH:
            if (stackPush(&processor->Stack, processor->Register[*(char *)getCmd(bytecode, 0)] - 1))
                {ERRLOG(RPUSH_CMD_FAIL, msgZeroArgs, "RPUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
            ipInc(bytecode, sizeof(char));
            break;

        case RAMPUSH:
            if (stackPush(&processor->Stack, *(int *)(processor->RAM + *(int *)getCmd(bytecode, 0))))
                {ERRLOG(RAMPUSH_CMD_FAIL, msgZeroArgs, "RAMPUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
            ipInc(bytecode, sizeof(int));
            break;

        default:
            ERRLOG(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
            break;
        }
        break;

    case ARITHM_GRP:
        if (arithmetics(&processor->Stack, cmd))
            {ERRLOG(ARITHM_CMD_FAIL, msgZeroArgs, "ARITHM_ERR", "", bytecode->ip, "[error]");}
        break; 

    case POP_GRP:
        switch (cmd)
        {
        case RPOP:
            if (stackPop(&processor->Stack, processor->Register + *getCmd(bytecode, 0) - 1))
                {ERRLOG(RPOP_CMD_FAIL, msgZeroArgs, "RPOP_CMD_FAIL", "", bytecode->ip, "[error]");}
            ipInc(bytecode, 0);
            break;

        case RAMPOP:
            if (stackPop(&processor->Stack, (dataType *)(processor->RAM + *(int *)getCmd(bytecode, 0))))
                {ERRLOG(RAMPOP_CMD_FAIL, msgZeroArgs, "RAMPOP_CMD_FAIL", "", bytecode->ip, "[error]");}
            ipInc(bytecode, sizeof(int));
            break;

        default:
            ERRLOG(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
            break;
        }
        break;
        
    case IN:
        printf("Enter the number:\n");
        if(!scanf("%d", &dataHolder))
            {ERRLOG(IN_CMD__SCAN_ERR, msgZeroArgs, "IN_CMD__SCAN_ERR", "", bytecode->ip, "[error]");}

        if(stackPush(&processor->Stack, dataHolder))
            {ERRLOG(IN_CMD__PUSH_FAIL, msgZeroArgs, "IN_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        break;

    case MOV_GRP:
        if(caseMov(processor, bytecode))
            {ERRLOG(MOV_CMD_FAIL, msgZeroArgs, "MOV_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;

    case OUT:
        printf("\n>>>DATA DUMPING:\n");
        while (stackPop(&processor->Stack, &dataHolder) != STK_EMPTY)    //пустой поп
            printf(">>> %d\n", dataHolder);    
        printf(">>> DUMP ENDED.\n");
        break;   

    case HALT:
        ERRLOG(PROGRAMM_ENDED, msgZeroArgs, "PROGRAMM_ENDED", "", bytecode->ip, "");
        return PROGRAMM_ENDED;
        break;
    
    case CALL:
        if (stackPush(&processor->returnPtrs, bytecode->ip + sizeof(int))) 
            {ERRLOG(CALL_CMD__PUSH_FAIL, msgZeroArgs, "CALL_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        jmp(bytecode);
        break;

    case JMP_GRP:
        caseJMP(&processor->Stack, bytecode, cmd);
        break;   

    case RET:
        if (stackPop(&processor->returnPtrs, &bytecode->ip)) 
            {ERRLOG(RET_CMD_FAIL, msgZeroArgs, "RET_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;

    default:
        ERRLOG(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        break;
    }

    return 0;
}

char *getCmd(const bytecode_t *bytecode, const signed int skipNum)
{
    assert(bytecode != NULL);

    return bytecode->bytecode + bytecode->ip + skipNum;
}

int ipInc(bytecode_t *bytecode, const unsigned int incNum)
{
    assert(bytecode != NULL);

    return bytecode->ip += incNum;
}

void fileLog(const char *format, ...)
{
    //fprintf(stderr, "format = '%s'", format);
    assert(logFile != NULL);

    va_list arg = {};
    va_start(arg, format);
    vfprintf(logFile, format, arg);
    va_end(arg);

    fflush(logFile);
}