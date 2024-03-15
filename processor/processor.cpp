#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "../stack/stack.h"
#include "../assembler/asm.h"
#include "../onegin/fileReader.h"
#include "processor.h"
#include "../common/log.h"

#define SIZE_OF_RAM 256

#ifdef DEBUG
#define ERRLOGRET(arg, ...)         \
    fileLog(logFile, __VA_ARGS__);           \
    return arg
#else
#define ERRLOGRET(arg, ...)    return arg
#endif

#define ARITHM(arg)                                                                     \
    do {                                                                                \
        if (stackName->stackPush(temp1 arg temp2))                                      \
        {                                                                               \
            ERRLOGRET(ARITHM_GRP_ERR, msgZeroArgs, "ARITHM_GRP_ERR", "", 0, "[error]"); \
            stackName->stackPrint(IN_CONSOLE);                                          \
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
    class stack <stackData_t> stack;
    class stack <int> returnPtrs;
    stackData_t RAM[SIZE_OF_RAM];
    stackData_t Register[REGISTERS_NUM];
} processor_t;

typedef struct
{
    char *bytecode;
    int bytecodeSize;
    int ip;
} bytecode_t;

FILE *logFile = NULL;

int arithmetics(stack <stackData_t> *stackName, const int lowerMask);
int caseJMP(stack <stackData_t> *stackName, bytecode_t *bytecode, const int cmd, const int address);
void fileLog(const char *format, ...);
char *getCmd(const bytecode_t *bytecode, const signed int skipNum);
int ipInc(bytecode_t *bytecode, const stackData_t incNum);
int doCommand(processor_t *processor, bytecode_t *bytecode);
char *getCmdAndInc(bytecode_t *bytecode, const int numSize);
void takeOneArgument(processor_t *SPU, bytecode_t *bytecode, uint8_t argFlags, stackData_t **argument);


int main(const int argc, const char** argv)
{
    assert(argc >= 2);
    bytecode_t bytecode = {0};

    fileRead(argv[1], &bytecode.bytecode, NULL, &bytecode.bytecodeSize, NULL, BUFF_ONLY);

#ifdef DEBUG
    const char *log1 = NULL;
    const char *log2 = NULL;

    logFile = openLogFile(3, argv, "processorErrorLog.log", "%-25s| %-17s| %-20s| %-20s|\n", "MESSAGE", "ARGUMENT", "IP", "ERROR");
#endif

    processor_t processor = {};

    processor.stack.stackCtor(10, "dataStack.log");
    processor.returnPtrs.stackCtor(5, "returnStack.log");

    int errorNum = 0;
    for (; bytecode.ip < bytecode.bytecodeSize;)
        if (errorNum = doCommand(&processor, &bytecode))
            break;
            
        
    processor.stack.stackDtor();
    processor.returnPtrs.stackDtor();
    free(bytecode.bytecode);
#ifdef DEBUG
    fclose(logFile);
#endif

    return errorNum;
}

int arithmetics(stack <stackData_t> *stackName, const int cmd)
{
    assert(stackName != NULL);
    stackData_t temp1 = 0;
    stackData_t temp2 = 0;

    if (stackName->stackPop(&temp2) || stackName->stackPop(&temp1))
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

int caseJMP(stack <stackData_t> *stackName, bytecode_t *bytecode, const int cmd, const int address)
{
    assert(stackName != NULL);
    assert(bytecode != NULL);
    stackData_t temp1 = 0;
    stackData_t temp2 = 0;

    if (stackName->stackPop(&temp2) || stackName->stackPop(&temp1))
        return -1;

    if (stackName->stackPush(temp1))
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

void takeOneArgument(processor_t *SPU, bytecode_t *bytecode, uint8_t argFlags, stackData_t **argument)
{   
    assert(SPU);
    assert(bytecode);
    assert(argument);

    if (argFlags & NUM_ARG && argFlags & REG_ARG)
    {

        stackData_t regHolder = SPU->Register[*getCmdAndInc(bytecode, sizeof(uint8_t))];
        stackData_t cmdHolder = *(stackData_t *)getCmdAndInc(bytecode, sizeof(stackData_t));

        **argument = regHolder + cmdHolder;
    }

    else if (argFlags & NUM_ARG)
        *argument = (stackData_t *)getCmdAndInc(bytecode, sizeof(stackData_t));

    else if (argFlags & REG_ARG)  
        *argument = (stackData_t *)&SPU->Register[*getCmdAndInc(bytecode, sizeof(uint8_t))];

    else if (argFlags & ADR_ARG)
        *argument = (stackData_t *)getCmdAndInc(bytecode, sizeof(int));

    else
        {ERRLOGRET(, msgZeroArgs, "ARG_TAKE_FAIL", "", bytecode->ip, "[error]");}

    if (argFlags & RAM_ARG)
        *argument = (stackData_t *)&SPU->RAM[(int)**argument];

        return;

}

int doCommand(processor_t *processor, bytecode_t *bytecode)
{
    assert(processor != NULL);
    assert(bytecode != NULL);    

    uint8_t cmd = *getCmdAndInc(bytecode, sizeof(uint8_t));

    static stackData_t valueHolder1 = 0;
    static stackData_t valueHolder2 = 0;

    stackData_t *argHolder1 = &valueHolder1;
    stackData_t *argHolder2 = &valueHolder2;
    
    int n = 0;

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

    switch (cmd)
    {
    case PUSH_GRP:
        if (processor->stack.stackPush(*argHolder1))
            {ERRLOGRET(PUSH_CMD_FAIL, msgZeroArgs, "PUSH_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;
    
    case ADD:
    case SUB:
    case MULT:
    case DIV:
        arithmetics(&processor->stack, cmd);
        break;
    
    case POP_GRP:
        if (processor->stack.stackPop(argHolder1))
            {ERRLOGRET(RPOP_CMD_FAIL, msgZeroArgs, "RPOP_CMD_FAIL", "", bytecode->ip, "[error]");}
        break;
    
    case IN:
        printf("Enter the number:\n");
        if(!scanf(DATA_SPEC, argHolder1, &n))
            {ERRLOGRET(IN_CMD__SCAN_ERR, msgZeroArgs, "IN_CMD__SCAN_ERR", "", bytecode->ip, "[error]");}

        if(processor->stack.stackPush(*argHolder1))
            {ERRLOGRET(IN_CMD__PUSH_FAIL, msgZeroArgs, "IN_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        break;

    case MOV_GRP:
        *argHolder1 = *argHolder2;
        break;

    case OUT:
    {
        stackData_t popData = 0;

        printf("\n>>>DATA DUMPING:\n");
        while (processor->stack.stackPop(&popData) != STK_EMPTY)    //пустой поп
            data_print(stderr, popData);
        printf(">>> DUMP ENDED.\n");
        break; 
    }
        
    case HALT:
        ERRLOGRET(PROGRAMM_ENDED, msgZeroArgs, "PROGRAMM_ENDED", "", bytecode->ip, "");
        break;

    case CALL:
        if (processor->returnPtrs.stackPush(bytecode->ip))
            {ERRLOGRET(CALL_CMD__PUSH_FAIL, msgZeroArgs, "CALL_CMD__PUSH_FAIL", "", bytecode->ip, "[error]");}
        //fprintf(stderr, "\n\n\n<<%d>>\n\n\n", *argHolder1);
        bytecode->ip = *(int *)argHolder1;
        break;

    case JMP:
    case JB:
    case JBE:
    case JA:
    case JAE:
    case JE:
    case JNE:
        caseJMP(&processor->stack, bytecode, cmd, *argHolder1);
        break;

    case RET:
        if (processor->returnPtrs.stackPop(&bytecode->ip)) 
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

int ipInc(bytecode_t *bytecode, const stackData_t incNum)
{
    assert(bytecode != NULL);

    return bytecode->ip += incNum;
}