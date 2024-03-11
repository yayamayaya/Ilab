#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include "fileRead.h"
#include "../stack/stack.h"
#include "asm.h"
#include "../common/log.h"

#ifdef DEBUG
    #define LOG(...) fileLog(logFile, __VA_ARGS__)
    #define LOG_FCLOSE() fclose(logFile)
#else
    #define LOG(...)
    #define LOG_FCLOSE() 
#endif

#if 0
    #define LABELS_PRINT(arg) labelTablePrint(arg)
#else
    #define LABELS_PRINT(arg)
#endif

#define ERROR_CHECK(arg) if(arg) return COMP_ERR
    

FILE *logFile = NULL;    

const char *msgZeroArgs = "%-25s| %-17s| %-20d| %-20s|\n";
const char *msgOneArgD = "%-25s| %-17d| %-20d| %-20s|\n";
const char *msgOneArgX = "%-25s| 0x%-15.2X| %-20d| %-20s|\n";

typedef struct 
{
    char *name;
    int ip;
} label_t;

typedef struct
{
    label_t *labelArr;   
    int labelTableSize;     
    int labelCounter;
} labelTable_t;

typedef struct
{
    uint8_t *bytecodeHolder;
    int bytecodeHolderSize;
    int ip;
} bytecode_t;

typedef struct 
{
    char **tokenArr;
    int tokenNumber;
} tokens_t;

static void *reallocUp(void *arrName, int *arrSize, int dataSize);
static enum ASM cmdDet(const cmd *commands, const int commandsNumber, const char *token);
static int argumentParsing(char *argToken, bytecode_t *bytecode);

static int argInArr(char *argToken, bytecode_t *bytecode);
static void cmdInArr(const byte_t cmdNum, bytecode_t *bytecode);
static void numInArr(void *numberPointer, const int sizeOfNumber, bytecode_t *bytecode);

static int caseOneArgument(char *argToken, int commandGroup, bytecode_t *bytecode);
static int caseMov(char **argPtr, bytecode_t *bytecode);

static int Compiler(const tokens_t *tokens, bytecode_t *bytecode, labelTable_t *labels);

static int labeldet(const labelTable_t *labels, const char *token);
static int labelInsert(labelTable_t *labels, char *labelName, const int ip);
static void labelTablePrint(const labelTable_t *labels);


#define MEM_FREE()                              \
    free(bytecode.bytecodeHolder);              \
    free(*tokens.tokenArr);                     \
    free(tokens.tokenArr);                      \
    free(labels.labelArr);                      \
    LOG_FCLOSE()
    
int main(const int argc, const char* argv[])
{
    assert(argc >= 3);

#ifdef DEBUG
    logFile = openLogFile(4, argv, "logFile.log", "%-25s| %-17s| %-20s| %-20s|\n\n", "INSTRUCT. NAME", "ARGUMENTS", "INSTRUCT. POINTER", "ERROR");
    if (!logFile)
        return FILENULL_ERR;
#endif

    if (argv[1] == NULL)
    {
        LOG(msgZeroArgs, "NULL_PTR", "", 0, "[error]");
        return FILEREAD_ERR;
    }

    bytecode_t bytecode = {(uint8_t *)calloc(100, sizeof(char)), 100, 0};
    labelTable_t labels = {(label_t *)calloc(10, sizeof(label_t)), 10, 0};
    tokens_t tokens = {0};

    if (bytecode.bytecodeHolder == NULL || labels.labelArr == NULL)
    {
        LOG(msgZeroArgs, "MEM_ALC_ERR", "", 0, "[error]");
        free(bytecode.bytecodeHolder);
        free(labels.labelArr);
        LOG_FCLOSE();
        return MEM_ALC_ERR;
    }  

    fileRead(argv[1], &tokens.tokenArr, &tokens.tokenNumber);
    if (tokens.tokenArr == NULL)
    {
        LOG(msgZeroArgs, "FILEREAD_ERR", "", 0, "[error]");
        MEM_FREE();
        return FILEREAD_ERR;
    }
    LOG(msgZeroArgs, "FILEREAD_OK", "", 0, "");
    
    int error = Compiler(&tokens, &bytecode, &labels);

    if(error)
    {
        fprintf(stderr, ">>errorNum = %d", error);
        fprintf(stderr, ">>Compilation's not successfull, please check your syntax and code errors.\n");
        MEM_FREE();
        return COMP_ERR;
    }
    LABELS_PRINT(&labels);
    if (labels.labelCounter != 0)
    { 
        bytecode.ip = 0;
        error = Compiler(&tokens, &bytecode, &labels);
        if(error)
        {
            fprintf(stderr, ">>errorNum = %d", error);
            fprintf(stderr, ">>Compilation's not successfull, please check your syntax and code errors.\n");
            MEM_FREE();
            return COMP_ERR;
        }
    }
    
    FILE * byteCode = fopen(argv[2], "wb");
    if (byteCode == NULL)
    {
        LOG(msgZeroArgs, "BYTECODE_FILEOPEN_ERR", "", bytecode.ip, "[error]");
        MEM_FREE();
        return FILEOPEN_ERR;
    }

    if(fwrite(bytecode.bytecodeHolder, sizeof(char), bytecode.ip, byteCode) != bytecode.ip)
    {
        LOG(msgZeroArgs, "BYTECODE_WRITE_ERR", "", 0, "[error]");
        MEM_FREE();
        fclose(byteCode);
        return FILEWRITE_ERR;        
    }
    
    fclose(byteCode);
    LOG(msgOneArgD, "COMP_TIME", clock(), bytecode.ip, "");
    MEM_FREE();     //Разделить на goto или через структуру

    return 0;
}

#undef MEM_FREE

void *reallocUp(void *arrName, int *arrSize, int dataSize)
{
    assert(arrName != NULL);
    assert(arrSize != 0);
    assert(dataSize != 0);

    *arrSize *= 2; 
    void *newArrPointer = realloc(arrName, *arrSize * dataSize);
    if (newArrPointer == NULL)
    {
        LOG(msgZeroArgs, "MEM_RLC_ERR", "", 0, "[error]");
        return arrName;        
    }

    LOG(msgZeroArgs, "MEM_RLC_OK", "", 0, "");
    return newArrPointer;
}

enum ASM cmdDet(const cmd *commands, const int commandsNumber, const char *token)
{
    assert(token);

    for (int pos = 0; pos < commandsNumber; pos++)
        if (strcmp(commands[pos].commandName, token) == 0)
            return commands[pos].commandNum;
    
    return NOCMD;
}

int argumentParsing(char *argToken, bytecode_t *bytecode)
{
    assert(bytecode);
    assert(argToken);

    char regHolder = 0;
    dataType dataHolder = 0;
    int intHolder = 0;
    int n = 0;

    sscanf(argToken, "%cx+%d%n", &regHolder, &intHolder, &n);
    if (n)
    {
        cmdInArr(regHolder - 'a', bytecode);
        numInArr(&intHolder, sizeof(int), bytecode);
        return REGNUM_ARG;
    }
    sscanf(argToken, "%cx%n", &regHolder, &n);
    if (n)
    {
        assert('a' <= regHolder && regHolder <= 'd');
        cmdInArr(regHolder - 'a', bytecode);
        return REG_ARG;
    }
    sscanf(argToken, DATA_SPEC, &dataHolder, &n);
    if (n)
    {
        numInArr(&dataHolder, sizeof(dataType), bytecode);
        return NUM_ARG;
    }
    
    LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
    return FATAL_ERR;  
} 

//--------------------------------------------Функции записи команд-------------------------------------------------------------

int argInArr(char *argToken, bytecode_t *bytecode)
{
    assert(argToken != NULL); 
    assert(bytecode != NULL);

    int ramMask = 0;
    int argMask = 0;
    
    int len = strlen(argToken) - 1;
    if (len < 0)
        len = 0;

    if (*argToken == '[' && argToken[len] == ']')
    {
        argToken++;
        argToken[len] = '\0';

        ramMask = RAM_ARG;
    }

    argMask = argumentParsing(argToken, bytecode);
    if (argMask == FATAL_ERR)
    {
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }
        
    return ramMask | argMask;   
}


void cmdInArr(const byte_t cmdNum, bytecode_t *bytecode)
{
    assert(bytecode);

    bytecode->bytecodeHolder[bytecode->ip] = cmdNum;
    LOG(msgOneArgX, "CMD_PUSHED_IN_BYTECODE", cmdNum, bytecode->ip, "");

    bytecode->ip++;
}

void numInArr(void *numberPointer, const int sizeOfNumber, bytecode_t *bytecode)
{
    assert(bytecode != NULL);
    assert(numberPointer != NULL);

    memcpy(bytecode->bytecodeHolder + bytecode->ip, numberPointer, sizeOfNumber);
    LOG(msgOneArgD, "NUMBER_PUSHED_IN_BYTECODE", *(int *)numberPointer, bytecode->ip, "");
    bytecode->ip += sizeOfNumber;
}

//--------------------------------------------Функции для основных групп команд-----------------------------------------

int caseOneArgument(char *argToken, int commandGroup, bytecode_t *bytecode)
{
    assert(argToken != NULL);
    assert(bytecode != NULL);

    uint16_t *commandPointer = (uint16_t *)(bytecode->bytecodeHolder + bytecode->ip);
    bytecode->ip += sizeof(uint16_t);

    int argMask = argInArr(argToken, bytecode);
    if (argMask == FATAL_ERR)
        return FATAL_ERR;

    *commandPointer = commandGroup | (argMask << 8);

    return 0;
}

int caseMov(char **argPtr, bytecode_t *bytecode)    
{
    assert(argPtr != NULL);
    assert(bytecode != NULL);

    uint16_t *commandPointer = (uint16_t *)(bytecode->bytecodeHolder + bytecode->ip);
    bytecode->ip += sizeof(uint16_t);

    int argMask1 = argInArr(*argPtr, bytecode);
    if (argMask1 == FATAL_ERR)
    {
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }
    else if (argMask1 == NUM_ARG)
    {
        LOG(msgZeroArgs, "MOV_WRONG_ARG", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    int argMask2 = argInArr(*(argPtr + 1), bytecode);
    if (argMask2 == FATAL_ERR)
    {
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    *commandPointer = (MOV_GRP) | ((argMask1 << 4 | argMask2) << 8);

    return 0;
}

//---------------------------------------------------Компилятор---------------------------------------------------------------

int Compiler(const tokens_t *tokens, bytecode_t *bytecode, labelTable_t *labels) 
{
    assert(tokens != NULL);
    assert(bytecode != NULL);
    assert(labels != NULL);

    for (int tokenNum = 0; tokenNum < tokens->tokenNumber; tokenNum++)
    {
        int labelIP = 0;
        uint16_t jmpCmd = 0;

        if(*tokens->tokenArr[tokenNum] == ';')    
            continue;
        
        char *colonPointer = NULL;
        if ((colonPointer = strchr(tokens->tokenArr[tokenNum], ':')) != NULL) 
        {
            *colonPointer = '\0';
            labelInsert(labels, tokens->tokenArr[tokenNum], bytecode->ip);
            continue;
        }
        labelIP = labeldet(labels, tokens->tokenArr[tokenNum]);
        if (labelIP) 
                continue;

        if (bytecode->ip >= bytecode->bytecodeHolderSize - 5 * (int)sizeof(long long int))
            bytecode->bytecodeHolder = (uint8_t *)reallocUp(bytecode->bytecodeHolder, &bytecode->bytecodeHolderSize, sizeof(char));
        
        byte_t cmdNum = cmdDet(commands, COMMANDS_NUM, tokens->tokenArr[tokenNum]);  
        
        switch (cmdNum)
        {
        case PUSH_GRP:
        case POP_GRP:
            ERROR_CHECK(caseOneArgument(tokens->tokenArr[++tokenNum], cmdNum, bytecode));
            break;

        case MOV_GRP:
            tokenNum++;           
            ERROR_CHECK(caseMov(tokens->tokenArr + tokenNum, bytecode));
            tokenNum++;
            break;

        /*case GRAPHICS:
            codeInArr(cmdNum, bytecodeHolder, &ip);
            int num = 0;
            sscanf(codeWords[++wordCounter] + 1, "%d", &num);
            numInArr(num, bytecodeHolder, &ip, sizeof(int));
            break;*/

        case CALL:
        case JMP:
        case JB:
        case JBE:
        case JA:
        case JAE:
        case JE:
        case JNE:
            tokenNum++;        
            jmpCmd = cmdNum | (ADR_ARG << 8);
            numInArr(&jmpCmd, (int)sizeof(uint16_t), bytecode);
            labelIP = labeldet(labels, tokens->tokenArr[tokenNum]);
            numInArr(&labelIP, (int)sizeof(int), bytecode);

            break;

        case NOCMD:
            LOG(msgZeroArgs, "COMP_ERR", "", bytecode->ip, "[error]");
            return COMP_ERR;
            break;

        case ADD:
        case SUB:
        case MULT:
        case DIV:
        case IN:
        case OUT:
        case HALT:
        case RET:
            cmdInArr(cmdNum, bytecode);
            break;
        
        default:
            LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
            return FATAL_ERR;
        } 
    }

    LOG(msgZeroArgs, "Compilation done", "", bytecode->ip, "");
    return 0;
}

//-----------------------------------------------LABELS------------------------------------------------------------

int labeldet(const labelTable_t *labels, const char *token)
{
    assert(labels != NULL);
    assert(token != NULL);

    for (int pos = 0; pos < labels->labelCounter; pos++)
    {
        assert(labels->labelArr[pos].name);

        if (strcmp(labels->labelArr[pos].name, token) == 0)
        {
            LOG(msgZeroArgs, "LABEL_DETECTED", labels->labelArr[pos].name, 00, "");                    
            return labels->labelArr[pos].ip;
        }
    }
        
    return 0;
}

int labelInsert(labelTable_t *labels, char *labelName, const int ip)
{
    assert(labels != NULL);
    assert(labelName != NULL);


    labels->labelArr[labels->labelCounter].name = labelName;
    labels->labelArr[labels->labelCounter].ip = ip;
    LOG(msgZeroArgs, "LABEL_INSERTED", labels->labelArr[labels->labelCounter].name, 00, "");   
    labels->labelCounter++;

    if (labels->labelCounter == labels->labelTableSize)
        labels->labelArr = (label_t *)reallocUp(labels->labelArr, &labels->labelTableSize, sizeof(label_t));    

    return 0; 
}

void labelTablePrint(const labelTable_t *labels)
{
    assert(labels != NULL);

    printf(">> Table size: %d\n", labels->labelTableSize);
    printf(">>Number of labels: %d\n", labels->labelCounter);
    for (int i = 0; i < labels->labelCounter; i++)
        printf(">> [%d] Label Name: %s, label address: %d\n", i, labels->labelArr[i].name, labels->labelArr[i].ip);
    printf("Table ended.\n");
}