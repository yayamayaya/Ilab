#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include "fileRead.h"
#include "../stack/stack.h"
#include "asm.h"
#include "log.h"

#ifdef DEBUG
    #define LOG(...) fileLog(__VA_ARGS__)
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
    char *bytecodeHolder;
    int bytecodeHolderSize;
    int ip;
} bytecode_t;

typedef struct 
{
    char **tokenArr;
    int tokenNumber;
} tokens_t;

//Написать статик
static void *reallocUp(void *array, int arrSize, int dataSize);

static enum ASM cmddet(const cmd *commands, const int commandsNumber, const char *word);
static int argDet(char *argToken, int *argHolder);

static void cmdInArr(const byte_t cmdNum, bytecode_t *bytecode);
static void numInArr(bytecode_t *bytecode, void *numberPointer, const int sizeOfNumber);
static void regInArr(const char *registerToken, bytecode_t *bytecode);

static int casePush(char *argToken, bytecode_t *bytecode);
static int casePop(char *argToken, bytecode_t *bytecode);
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
        if (argc == 4)
            logFile = fopen(argv[3], "w");
        else
            logFile = fopen("logFile.log", "w");
        
        if (logFile == NULL)
        {
            fprintf(stderr, "Can't open logFile.");
            return FILENULL_ERR;
        }
        LOG("%-25s| %-17s| %-20s| %-20s|\n\n", "INSTRUCT. NAME", "ARGUMENTS", "INSTRUCT. POINTER", "ERROR");
    #endif

    if (argv[1] == NULL)
    {
        LOG(msgZeroArgs, "NULL_PTR", "", 0, "[error]");
        return FILEREAD_ERR;
    }

    bytecode_t bytecode = {(char *)calloc(100, sizeof(char)), 100, 0};
    labelTable_t labels = {(label_t *)calloc(10, sizeof(label_t)), 10, 0};
    tokens_t tokens = {0};

    if (bytecode.bytecodeHolder == NULL)
    {
        LOG(msgZeroArgs, "MEM_ALC_ERR", "", 0, "[error]");
        free(labels.labelArr);
        LOG_FCLOSE();
        return MEM_ALC_ERR;
    }   
    if (labels.labelArr == NULL)
    {
        LOG(msgZeroArgs, "MEM_ALC_ERR", "", 0, "[error]");        
        free(bytecode.bytecodeHolder);
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

    if(Compiler(&tokens, &bytecode, &labels))
    {
        fprintf(stderr, ">>Compilation's not successfull, please check your syntax and code errors.\n");
        MEM_FREE();
        return COMP_ERR;
    }
    LABELS_PRINT(&labels);
    if (labels.labelCounter != 0)
    { 
        bytecode.ip = 0;
        if(Compiler(&tokens, &bytecode, &labels))
        {
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

    if(fwrite(bytecode.bytecodeHolder, (int)sizeof(char), bytecode.ip, byteCode) < bytecode.ip)
    {
        LOG(msgZeroArgs, "BYTECODE_WRITE_ERR", "", 0, "[error]");
        MEM_FREE();
        fclose(byteCode);
        return FILEWRITE_ERR;        
    }
    
    fclose(byteCode);
    LOG(msgOneArgD, "COMP_TIME", clock(), bytecode.ip, "");
    MEM_FREE();

    return 0;
}

#undef MEM_FREE

void *reallocUp(void *arrName, int *arrSize, int dataSize)       //Реаллокация
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

//--------------------------------------------Функции определения команд-------------------------------------------------------------

enum ASM cmddet(const cmd *commands, const int commandsNumber, const char *token)       //Определение команды
{
    for (int pos = 0; pos < commandsNumber; pos++)
        if (strcmp(commands[pos].commandName, token) == 0)
            return commands[pos].commandNum;
    
    return NOCMD;
}

int argDet(char *argToken, int *argHolder)          //Определение типа аргумента
{
    assert(argToken != NULL);
    assert(argHolder != NULL);

    if (sscanf(argToken, "%d", argHolder))
        return ARG_NUMBER;

    //else if (sscanf(argToken, "[%d]%n", argHolder, &charRead) == 2)
    else if (strchr(argToken, '[') != NULL && sscanf(argToken + 1, "%d", argHolder))        //Указать спецификаторы для аргументов
        if (strchr(argToken, ']') == NULL)
        {
            LOG(msgZeroArgs, "SYNTAX_ERR", "", 0, "[error]");
            return ARG_DET_ERR;
        }
        else
            return ARG_RAM;

    else if (strchr(argToken, 'x') != NULL)
        return ARG_REGISTER;

    
    return ARG_DET_ERR;
}

//--------------------------------------------Функции записи команд-------------------------------------------------------------

void cmdInArr(const byte_t cmdNum, bytecode_t *bytecode)        //Команда в массив
{
    bytecode->bytecodeHolder[bytecode->ip] = cmdNum;
    LOG(msgOneArgX, "CMD_PUSHED_IN_BYTECODE", cmdNum, bytecode->ip, "");

    bytecode->ip++;
}

void numInArr(bytecode_t *bytecode, void *numberPointer, const int sizeOfNumber)        //Число в массив
{
    assert(bytecode != NULL);
    assert(numberPointer != NULL);

    memcpy(bytecode->bytecodeHolder + bytecode->ip, numberPointer, sizeOfNumber);
    LOG(msgOneArgD, "NUMBER_PUSHED_IN_BYTECODE", *(int *)numberPointer, bytecode->ip, "");
    bytecode->ip += sizeOfNumber;
}

void regInArr(const char *registerToken, bytecode_t *bytecode)      //Регистр в массив
{
    assert(bytecode != NULL);
    assert(registerToken != NULL);

    int regNum = cmddet(registers, REGISTERS_NUM, registerToken);
    if (regNum & NOCMD)
        LOG(msgZeroArgs, "REGISTER_NOT_FOUND", "", bytecode->ip, "");
    
    cmdInArr(regNum, bytecode);  
}

//--------------------------------------------Функции для основных групп команд-----------------------------------------

int casePush(char *argToken, bytecode_t *bytecode)      //Команды пуш
{
    assert(argToken != NULL);
    assert(bytecode != NULL);

    int num = 0;

    switch (argDet(argToken, &num))
    {
    case ARG_NUMBER:
        cmdInArr(PUSH, bytecode);        
        numInArr( bytecode, &num, sizeof(dataType));
        break;

    case ARG_RAM:
        cmdInArr(RAMPUSH, bytecode);
        numInArr(bytecode, &num, sizeof(int));
        break;

    case ARG_REGISTER:
        cmdInArr(RPUSH, bytecode);
        regInArr(argToken, bytecode);
        break;

    case ARG_DET_ERR:
        LOG(msgZeroArgs, "ARGUMENT_DET_ERR", "", bytecode->ip, "[error]");
        return ARG_DET_ERR;
        break;

    default:
        printf("<%d>", argDet(argToken, &num));
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    return 0;
}

int casePop(char *argToken, bytecode_t *bytecode)       //Команды поп
{
    assert(argToken != NULL);
    assert(bytecode != NULL);

    int num = 0;

    switch (argDet(argToken, &num))
    {
    case ARG_RAM:
        cmdInArr(RAMPOP, bytecode);
        numInArr(bytecode, &num, sizeof(int));
        break;
    case ARG_REGISTER:
        cmdInArr(RPOP, bytecode);
        regInArr(argToken, bytecode);
        break;
    case ARG_DET_ERR:
        LOG(msgZeroArgs, "ARGUMENT_DET_ERR", "", bytecode->ip, "[error]");
        return ARG_DET_ERR;
        break;
    default:
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    return 0;
}

int caseMov(char **argPtr, bytecode_t *bytecode)     //Команды мов
{
    assert(argPtr != NULL);
    assert(bytecode != NULL);

    int cmdNum = MOV_GRP;
    char *commandPtr = bytecode->bytecodeHolder + bytecode->ip;
    (bytecode->ip)++;

    int num = 0;

    switch (argDet(*argPtr, &num))
    {
    case 2:
        cmdNum |= RAM_FRST;
        numInArr(bytecode, &num, sizeof(int));
        break;
    case 3:
        cmdNum |= REG_FRST;
        regInArr(*argPtr, bytecode);
        break;
    case ARG_DET_ERR:
        LOG(msgZeroArgs, "ARGUMENT_DET_ERR", "", bytecode->ip, "[error]");
        return ARG_DET_ERR;
        break;
    default:
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    switch (argDet(*(argPtr + 1), &num))
    {
    case 1:
        cmdNum |= 0x03;
        numInArr(bytecode, &num, sizeof(dataType));
        break;
    case 2:
        cmdNum |= 0x02;
        numInArr(bytecode, &num, sizeof(int));
        break;
    case 3:
        cmdNum |= 0x01;
        regInArr(*(argPtr + 1), bytecode);
        break;
    case ARG_DET_ERR:
        LOG(msgZeroArgs, "ARGUMENT_DET_ERR", "", bytecode->ip, "[error]");
        return ARG_DET_ERR;
        break;
    default:
        LOG(msgZeroArgs, "FATAL_ERROR", "", bytecode->ip, "[error]");
        return FATAL_ERR;
    }

    *commandPtr = cmdNum;
    return 0;
}

//---------------------------------------------------Компилятор---------------------------------------------------------------

int Compiler(const tokens_t *tokens, bytecode_t *bytecode, labelTable_t *labels)  //Передаём в компилятоор 3 основных аргумента: метки, токены, байткод
{
    assert(tokens != NULL);
    assert(bytecode != NULL);
    assert(labels != NULL);

    for (int tokenNum = 0; tokenNum < tokens->tokenNumber; tokenNum++)       //Компиляция
    {
        int labelIP = 0;

        if(*tokens->tokenArr[tokenNum] == ';')      //Комментарии
            continue;
        
        char *colonPointer = NULL;
        if ((colonPointer = strchr(tokens->tokenArr[tokenNum], ':')) != NULL)  //Ищем метку, вставляем в таблицу
        {
            *colonPointer = '\0';
            labelInsert(labels, tokens->tokenArr[tokenNum], bytecode->ip);
        }
        if (labelIP = labeldet(labels, tokens->tokenArr[tokenNum]))      //Ищем уже считанную метку, чтобы компилятор не выдал ошибки
                continue;

        if (bytecode->ip >= bytecode->bytecodeHolderSize - 5 * (int)sizeof(long long int))
            bytecode->bytecodeHolder = (char *)reallocUp(bytecode->bytecodeHolder, &bytecode->bytecodeHolderSize, sizeof(char));
        
        byte_t cmdNum = cmddet(commands, COMMANDS_NUM, tokens->tokenArr[tokenNum]);   //Ищем команду
        byte_t mask = cmdNum & 0xF0;               //Ставим маску
        //Делаем свитч
        
        switch (mask)
        {
        case PUSH_GRP:
            ERROR_CHECK(casePush(tokens->tokenArr[++tokenNum], bytecode));
            break;

        case POP_GRP:
            ERROR_CHECK(casePop(tokens->tokenArr[++tokenNum], bytecode));
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
        case JMP_GRP:
            cmdInArr(cmdNum, bytecode);
            ++tokenNum;
            labelIP = labeldet(labels, tokens->tokenArr[tokenNum]);
            numInArr(bytecode, &labelIP, sizeof(int));
            break;

        case NOCMD:
            LOG(msgZeroArgs, "COMP_ERR", "", bytecode->ip, "[error]");
            return COMP_ERR;
            break;

        case ARITHM_GRP:
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

    for (size_t pos = 0; pos < labels->labelCounter; pos++)
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
    for (size_t i = 0; i < labels->labelCounter; i++)
        printf(">> [%d] Label Name: %s, label address: %d\n", i, labels->labelArr[i].name, labels->labelArr[i].ip);
    printf("Table ended.\n");
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