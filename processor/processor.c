#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include "../stack/stack.h"
#include "../assembler/asm.h"
#include "../onegin/fileReader.h"
#include "processor.h"
#include "../common/log.h"

#define SIZE_OF_RAM 256

#ifdef DEBUG
#define ERRLOGRET(arg, ...)         \
    fileLog(__VA_ARGS__);           \
    return arg
#else
#define ERRLOGRET(arg, ...)    return arg
#endif

#define ARITHM(arg)                                                                     \
    do {                                                                                \
        if (stackPush(stackName, temp1 arg temp2))                                      \
        {                                                                               \
            ERRLOGRET(ARITHM_GRP_ERR, msgZeroArgs, "ARITHM_GRP_ERR", "", 0, "[error]"); \
            stackPrint(stackName, IN_CONSOLE);                                          \
        }                                                                               \
    } while(0)  

#define COMPARE(arg)                    \
    do                                  \
    {                                   \
        if (temp1 arg temp2)            \
        {                               \
            bytecode->ip = address;     \
            return 0;                   \
        }                               \
    } while (0)


const char *msgZeroArgs = "%-25s| %-17s| %-20d| %-20s|\n";
const char *msgOneArgD = "%-25s| %-17d| %-20d| %-20s|\n";
const char *msgOneArgX = "%-25s| 0x%-15.2X| %-20d| %-20s|\n";

typedef struct
{
    stack Stack;
    stack returnPtrs;
    dataType RAM[SIZE_OF_RAM];
    dataType Register[REGISTERS_NUM];
} processor_t;

typedef struct
{
    char *bytecode;
    int bytecodeSize;
    dataType ip;
} bytecode_t;

FILE *logFile = NULL;

int arithmetics(stack *stackName, const int lowerMask);
int caseJMP(stack *stackName, bytecode_t *bytecode, const int cmd, const int address);
void fileLog(const char *format, ...);
char *getCmd(const bytecode_t *bytecode, const signed int skipNum);
int ipInc(bytecode_t *bytecode, const dataType incNum);
void fileLog(const char *format, ...);
int doCommand(processor_t *processor, bytecode_t *bytecode);
char *getCmdAndInc(bytecode_t *bytecode, const int numSize);
void takeOneArgument(processor_t *SPU, bytecode_t *bytecode, uint8_t argFlags, dataType **argument);


int main(const int argc, const char** argv)
{
    assert(argc >= 2);
    bytecode_t bytecode = {0};

    fileRead(argv[1], &bytecode.bytecode, NULL, &bytecode.bytecodeSize, NULL, BUFF_ONLY);

    const char *log1 = NULL;
    const char *log2 = NULL;

#ifdef DEBUG
    logFile = openLogFile(3, argv, "processorErrorLog.log", "%-25s| %-17s| %-20s| %-20s|\n", "MESSAGE", "ARGUMENT", "IP", "ERROR");
#endif

    processor_t processor = {0};
    stackCtor(&processor.Stack, 10, "dataStack.log");
    stackCtor(&processor.returnPtrs, 5, "returnStack.log");

    int lessSignBitMask = 0;
    int errorNum = 0;
    for (; bytecode.ip < bytecode.bytecodeSize;)
        if (errorNum = doCommand(&processor, &bytecode))
            break;
            
        
    stackDtor(&processor.Stack);
    stackDtor(&processor.returnPtrs);
    free(bytecode.bytecode);
#ifdef DEBUG
    fclose(logFile);
#endif

    return errorNum;
}

int arithmetics(stack *stackName, const int cmd)
{
    assert(stackName != NULL);
    dataType temp1 = 0;
    dataType temp2 = 0;

    if (stackPop(stackName, &temp2) || stackPop(stackName, &temp1))
        return -1;
       
    switch (cmd)
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
        ERRLOGRET(FATAL_ERR, msgZeroArgs, "FATAL_ERROR", "ARITHM_GRP", 0, "[error]");
        break;
    }

    return 0;
}

int caseJMP(stack *stackName, bytecode_t *bytecode, const int cmd, const int address)
{
    assert(stackName != NULL);
    assert(bytecode != NULL);
    dataType temp1 = 0;
    dataType temp2 = 0;

    if (stackPop(stackName, &temp2) || stackPop(stackName, &temp1))
        return -1;

    if (stackPush(stackName, temp1))
        {ERRLOGRET(RPUSH_CMD_FAIL, msgZeroArgs, "RPUSH_CMD_FAIL", "", bytecode->ip, "[error]");}

    switch(cmd)
    {
    case JMP:
        bytecode->ip = address;
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
        ERRLOGRET(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        break;    
    }

    return 0;
}

void takeOneArgument(processor_t *SPU, bytecode_t *bytecode, uint8_t argFlags, dataType **argument)
{   
    assert(SPU);
    assert(bytecode);
    assert(argument);

    if (argFlags & 0b0001 && argFlags & 0b0010)
    {

        dataType regHolder = SPU->Register[*getCmdAndInc(bytecode, sizeof(uint8_t))];
        dataType cmdHolder = *(dataType *)getCmdAndInc(bytecode, sizeof(dataType));

        **argument = regHolder + cmdHolder;
    }

    else if (argFlags & 0b0001)
        *argument = (dataType *)getCmdAndInc(bytecode, sizeof(dataType));

    else if (argFlags & 0b0010)  
        *argument = (dataType *)&SPU->Register[*getCmdAndInc(bytecode, sizeof(uint8_t))];

    else
        {ERRLOGRET(, msgZeroArgs, "ARG_TAKE_FAIL", "", bytecode->ip, "[error]");}

    if (argFlags & 0b0100)
        *argument = (dataType *)&SPU->RAM[(int)**argument];

        return;

}

int doCommand(processor_t *processor, bytecode_t *bytecode)
{
    assert(processor != NULL);
    assert(bytecode != NULL);    

    uint8_t cmd = *getCmdAndInc(bytecode, sizeof(uint8_t));

    static dataType valueHolder1 = 0;
    static dataType valueHolder2 = 0;

    dataType *argHolder1 = &valueHolder1;
    dataType *argHolder2 = &valueHolder2;

    if (cmd & 0x80)
    {
        uint8_t argFlags = *getCmdAndInc(bytecode, sizeof(uint8_t));
        if (cmd & 0x10)
        {
            takeOneArgument(processor, bytecode, argFlags >> 4, &argHolder1);
            takeOneArgument(processor, bytecode, argFlags, &argHolder2);
        }
        else
            takeOneArgument(processor, bytecode, argFlags, &argHolder1);
    }
    /*if (argHolder1 != NULL)
        fprintf(stderr, "argHolder1 = %d (%d)\n", *argHolder1, bytecode->ip);
    
    if (argHolder2 != NULL)
        fprintf(stderr, "argHolder2 = %d (%d)\n", *argHolder2, bytecode->ip);*/

    switch (cmd)
    {
    case PUSH_GRP:
        if (stackPush(&processor->Stack, *argHolder1))
            {ERRLOGRET(PUSH_CMD_FAIL, msgZeroArgs, "PUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;
    
    case ADD:
    case SUB:
    case MULT:
    case DIV:
        arithmetics(&processor->Stack, cmd);
        break;
    
    case POP_GRP:
        if (stackPop(&processor->Stack, argHolder1))
            {ERRLOGRET(RPOP_CMD_FAIL, msgZeroArgs, "RPOP_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;
    
    case IN:
        printf("Enter the number:\n");
        if(!scanf("%d", argHolder1))
            {ERRLOGRET(IN_CMD__SCAN_ERR, msgZeroArgs, "IN_CMD__SCAN_ERR", "", bytecode->ip, "[error]");}

        if(stackPush(&processor->Stack, *argHolder1))
            {ERRLOGRET(IN_CMD__PUSH_FAIL, msgZeroArgs, "IN_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        break;

    case MOV_GRP:
        *argHolder1 = *argHolder2;
        break;

    case OUT:
    {
        dataType popData = 0;

        printf("\n>>>DATA DUMPING:\n");
        while (stackPop(&processor->Stack, &popData) != STK_EMPTY)    //пустой поп
            printf(">>> %d\n", popData);    
        printf(">>> DUMP ENDED.\n");
        break; 
    }
        
    case HALT:
        ERRLOGRET(PROGRAMM_ENDED, msgZeroArgs, "PROGRAMM_ENDED", "", bytecode->ip, "");
        break;

    case CALL:
        if (stackPush(&processor->returnPtrs, bytecode->ip))
            {ERRLOGRET(CALL_CMD__PUSH_FAIL, msgZeroArgs, "CALL_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        //fprintf(stderr, "\n\n\n<<%d>>\n\n\n", *argHolder1);
        bytecode->ip = *argHolder1;
        break;

    case JMP:
    case JB:
    case JBE:
    case JA:
    case JAE:
    case JE:
    case JNE:
        caseJMP(&processor->Stack, bytecode, cmd, *argHolder1);
        break;

    case RET:
        if (stackPop(&processor->returnPtrs, &bytecode->ip)) 
            {ERRLOGRET(RET_CMD_FAIL, msgZeroArgs, "RET_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;


    default:
        fprintf(stderr, "ERROR_CMD = %d\n\n", cmd);
        ERRLOGRET(FATAL_ERROR, msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        break;
    }

    return 0;
}

char *getCmdAndInc(bytecode_t *bytecode, const int numSize)
{
    assert(bytecode != NULL);
    assert(numSize > 0);

    char *Holder = getCmd(bytecode, 0);
    ipInc(bytecode, numSize);
    return Holder;
}

char *getCmd(const bytecode_t *bytecode, const signed int skipNum)
{
    assert(bytecode != NULL);

    return bytecode->bytecode + (int)bytecode->ip + skipNum;
}

int ipInc(bytecode_t *bytecode, const dataType incNum)
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