#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../common/log.h"
#include "stack.h"

#ifndef STACK
#define STACK

#ifdef SECURE
#define STACK_SIZE                      capacity * sizeof(T) + 2 * sizeof(canary_t) + sizeof(hash_t)
#define STACK_DATA_POSITION             sizeof(canary_t) + sizeof(hash_t)
#define HASH_REFRESH()                    *(getHashNumberPtr()) = hashFunc()
#else
#define STACK_DATA_POSITION             0
#define STACK_SIZE                      capacity * sizeof(T)
#define HASH_REFRESH()                  do {} while (0)
#endif

#ifdef DEBUG
#define STACK_LOG(...) fileLog(this->logFile, __VA_ARGS__) 
#define LOG_FILE_CLOSE()               \
    fclose(logFile);        \
    logFile = NULL
#define DUMP_TO_LOGFILE()   if (!option) fileName = logFile
#else
#define DUMP_TO_LOGFILE()   do {} while (0)
#define STACK_LOG(...)
#define LOG_FILE_CLOSE()
#endif

//const stackData_t poison = 0xDD;
const canary_t Lcanary = 0xDEDDEAD;
const canary_t Rcanary = 0xFFABEBA;

extern const char *msgNoArgs;
extern const char *msgOneArg;

void data_print(FILE *fileName, const char num);
void data_print(FILE *fileName, const int num);
void data_print(FILE *fileName, const double num);
void data_print(FILE *fileName, const long long int num);

//#ifdef SECURE

template <typename T>
canary_t *stack<T>:: getLeftCanaryPtr()
{
    return (canary_t *)data;
}

template <typename T>
hash_t *stack<T>:: getHashNumberPtr()
{
    return (hash_t *)((char *)data + sizeof(canary_t));
}

template <typename T>
T *stack<T>:: getDataPtr()
{
    return (T *)((char *)data + STACK_DATA_POSITION);
}

template <typename T>
canary_t *stack<T>:: getRightCanaryPtr()
{
    return (canary_t *)((char *)data + sizeof(canary_t) + sizeof(hash_t) + capacity * sizeof(T));
}

template <typename T>
int stack<T>::stackCtor(const int stackCapacity, const char* logFileName)
{
    assert(stackCapacity != 0);

#ifdef DEBUG
    assert(logFileName != NULL);
    this->logFile = fopen(logFileName, "w");
    if (this->logFile == NULL)
    {
        printf("[error]>>Can't open stack log.\n");
        return LOG_FILE_OPEN_ERR;     
    }
    fileLog(this->logFile, "%-25s| %-20s| %-20s|\n\n", "INSTRUCT. NAME", "ARGUMENTS", "ERROR");
#endif

    capacity = stackCapacity;
    data = (T *)calloc(STACK_SIZE, 1);
    //fprintf(stderr, "Stack size: %d; Stack pointer: %p\n", STACK_SIZE, data);
    if (data == NULL)
    {
        STACK_LOG(msgNoArgs, "STK_MEM_ALC_ERR", "", "[error]");
        LOG_FILE_CLOSE();
        return STK_MEM_ALC_ERR;
    }
    
#ifdef SECURE
    *getLeftCanaryPtr() = Lcanary;
    *getHashNumberPtr() = hashFunc();
    *getRightCanaryPtr() = Rcanary;
#endif

    size = 0;
    //capacity = stackCapacity;
    poisonFunc();   

    STACK_LOG(msgNoArgs, "STK_CONSTED", "", ""); 
    return stackVerificator();
}

template <typename T>
int stack<T>::stackDtor()
{
    int errorNum = stackVerificator();
    
    capacity = 0;
    size = 0;

    free(data);

    data = NULL;
    STACK_LOG(msgNoArgs, "STK_DESTTED", "", "");
    LOG_FILE_CLOSE();

    return 0;
}

template <typename T>
int stack<T>::stackPush(const T num)
{
    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (size == capacity)           //Проверка на переполнение массива
        if(stk_realloc(UP))
            return STK_MEM_RLC_ERR;
    
    *(getDataPtr() + size) = num;
    //data[this->size] = num;
    STACK_LOG("%-25s| %-20lld| %-20s|\n", "STK_PUSH", *(getDataPtr() + size), "");
    size++;

    HASH_REFRESH();
    
    return stackVerificator();
}

template <typename T>
int stack<T>::stackPop(T *num)
{    
    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (size == 0)
    {
        STACK_LOG(msgNoArgs, ">> Stack is empty", "STK_EMPTY", "");
        return STK_EMPTY;
    }

    --size;   
    if (num != NULL)     //Попаем значение, затем заполняем его ядом
        *num = *(getDataPtr() + size);
    *(getDataPtr() + size) = poison;

    HASH_REFRESH();

    STACK_LOG(msgNoArgs, ">> Number was popped", "STK_POP", "");
    return stackVerificator();
}

template <typename T>
int stack<T>::stackPrint(int option)
{
    FILE *fileName = stdout;
    setbuf(stdout, NULL);    
    DUMP_TO_LOGFILE();

    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    fprintf(fileName, "--------------------------------------------------------------------------------\n");
    fprintf(fileName, "\n>> Stack dump:\n");
    fprintf(fileName, "\n>> Size: %d", size);
    fprintf(fileName, "\n>> Capacity: %d", capacity);
    fprintf(fileName, "\n>> Current data in stack:\n");    
    for (int i = 0; i < size; i++)
    {
        fprintf(fileName, "%d) ", i + 1);
        data_print(fileName, *(getDataPtr() + i));        
    }

#ifdef SECURE
    fprintf(fileName, ">> Poison is: ");
    data_print(fileName, poison);
    CANARYPRINT(*(getLeftCanaryPtr()), *(getRightCanaryPtr()));
    fprintf(fileName, ">> Class canaries:\n");
    CANARYPRINT(classLCanary, classRCanary);
    HASHPRINT(*(getHashNumberPtr()));
#endif

    fprintf(fileName, "--------------------------------------------------------------------------------\n");

    return 0;
}

template <typename T>
int stack<T>::stk_realloc(const int num)
{
    assert(num == UP || num == DOWN);

    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (num == UP)
    {
        capacity *= 2;
        STACK_LOG(msgNoArgs, ">> Capacity doubled", "CAPACITY_UP", "");
    }
    else if(num == DOWN)
    {
        if (capacity == 1)
        {
            STACK_LOG(msgNoArgs, ">> Memory reallocatiom error", "STK_MEM_RLC_ERR", "[error]");
            return STK_MEM_RLC_ERR;
        }
        
        capacity /= 2;
        STACK_LOG(msgNoArgs, ">> Capacity halfed", "CAPACITY_DOWN", "");
    }

    T *memHolder = (T *)realloc(data, STACK_SIZE);
    if (!memHolder)
    {
        STACK_LOG(msgNoArgs, ">> Memory reallocatiom error", "STK_MEM_RLC_ERR", "[error]");
        return STK_MEM_RLC_ERR;
    }
    data = memHolder;
    *getRightCanaryPtr() = Rcanary;

    if (num == UP)
        if (poisonFunc())
        return PSN_ERR;
    
    HASH_REFRESH();

    return stackVerificator();
}

template <typename T>
hash_t stack<T>::hashFunc()
{
    hash_t hashHolder = 0;
    for (int i = 0; i < size; i++)
        hashHolder += *(getDataPtr() + i) * size;
    hashHolder += capacity;
    hashHolder += size;
    
    return hashHolder;
}

template <typename T>
int stack<T>::poisonFunc()
{   
    for (int i = size; i < capacity; i++)
    {
        //fprintf(stderr, "dataPtr: %p, data before: %d\n\n", getDataPtr() + i, *(getDataPtr() + i));
        *(getDataPtr() + i) = poison;
        //fprintf(stderr, "data after: %d\n\n", *(getDataPtr() + i));
    }

    return 0;
}

template <typename T>
int stack<T>::poisonCheck()
{
    for (int pos = size; pos < capacity; pos++)
        if (*(getDataPtr() + pos) != poison)
            return PSN_ERR;
  
    return 0;
}

template <typename T>
int stack<T>::canaryCheck()
{
    if (*(getLeftCanaryPtr()) != Lcanary || *(getRightCanaryPtr()) != Rcanary)
        return CANARY_ERR;
        
    return 0;
}

template <typename T>
int stack<T>::hashCheck()
{
    if (*(getHashNumberPtr()) != hashFunc())
        return HASH_ERR;

    return 0;
}

template <typename T>
int stack<T>::stackVerificator()
{
#ifdef SECURE
    if (capacity == 0)
    {
        STACK_LOG(msgNoArgs, ">> Capacity can't be zero", "CAPACITY_ZERO", "[error]", "");
        return CAPACITY_ZERO;
    }
    if (poisonCheck())
    {
        STACK_LOG(msgNoArgs, ">> Number is not poison", "PSN_ERR", "[error]", "");
        return PSN_ERR;
    }
    if (canaryCheck())
    {
        STACK_LOG(msgNoArgs, ">> Canary is damaged. ", "CANARY_ERR", "[error]", "");
        return CANARY_ERR;
    }
    if (hashCheck())
    {
        STACK_LOG(msgNoArgs, ">> Hash is not matching.", "HASH_ERR", "[danger]", "");
        return HASH_ERR;
    }
    if (classLCanary != CLASS_L_CANARY_VAL || classRCanary != CLASS_R_CANARY_VAL)
    {
        STACK_LOG(msgNoArgs, ">> Class canary is damaged. ", "CANARY_ERR", "[error]", "");
        return CANARY_ERR;
    }
#endif  
    return 0;
}

#endif