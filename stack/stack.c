#include "stack.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef DEBUG
#define LOG(...) stackFileLog(pstk->logFile, __VA_ARGS__) 
#define FILECLOSE()               \
    fclose(pstk->logFile);        \
    pstk->logFile = NULL
#else
#define LOG(...)
#define FILECLOSE()
#endif

const char *msgNoArgs = "%-25s| %-20s| %-20s|\n";
const char *msgOneArg = "%-25s| %-20d| %-20s|\n";

hash_t hashFunc(const stack *pstk);
int poisonFunc(stack *pstk);
int poisonCheck(const stack *pstk);
int canaryCheck(const stack *pstk);
int hashCheck(const stack *pstk);
int stackVerificator(const stack *pstk);
void stackFileLog(FILE *fileName, const char *format, ...) __attribute__ ((format(fprintf, 2, 3)));;

const dataType poison = 0xDD;
const canary Lcanary = 0xDEDDEAD;
const canary Rcanary = 0xFFABEBA;

int stackCtor(stack *pstk, const int capacity, const char* logFileName)
{
    assert(pstk != NULL);
    assert(capacity != 0);

#ifdef DEBUG            //Открываем логфайл
    assert(logFileName != NULL);
    pstk->logFile = fopen(logFileName, "w");
    if (pstk->logFile == NULL)  //Проверка на нулевой указате, ""ль
    {
        printf("[error]>>Can't open stack log.\n");
        //#undef DEBUG        
    }
    stackFileLog(pstk->logFile, "%-25s| %-20s| %-20s|\n\n", "INSTRUCT. NAME", "ARGUMENTS", "ERROR");
#endif

#ifdef SECURE
    pstk->leftCanaryPtr = (canary *)calloc(capacity * sizeof(dataType) + 2 * sizeof(canary) + sizeof(hash_t), 1);       
    if (pstk->leftCanaryPtr == NULL)        //Аллоцируем память под канареек, хеш и данные, присваиваем адреса памяти канарейкам и данным
    {
        LOG(msgNoArgs, "MEM_ALC_ERR", "", "[error]");
        return MEM_ALC_ERR;
    }

    pstk->hash = (hash_t *)(pstk->leftCanaryPtr + 1);
    pstk->data = (dataType *)(pstk->hash + 1);
    pstk->rightCanaryPtr = (canary *)(pstk->data + capacity);

    *(pstk->leftCanaryPtr) = Lcanary;   //Присваиваем канарейкам значения
    *(pstk->rightCanaryPtr) = Rcanary; 
#else
    pstk->data = (dataType *)calloc(capacity, sizeof(dataType));
    if(pstk->data == NULL)
    {
        LOG(msgNoArgs, "MEM_ALC_ERR", "", "[error]");
        return MEM_ALC_ERR;
    }
#endif

    pstk->size = 0;
    pstk->capacity = capacity;

    if (poisonFunc(pstk))
        return PSN_ERR;
    
    *(pstk->hash) = hashFunc(pstk);   

    LOG(msgNoArgs, "STK_CONSTED", "", ""); 
    return stackVerificator(pstk);
}

int stackDtor(stack *pstk)
{
    assert(pstk != NULL);

    int errorNum = 0;
    if (errorNum = stackVerificator(pstk))
        return errorNum;
    
    pstk->capacity = 0;
    pstk->size = 0;

#ifdef SECURE
    free(pstk->leftCanaryPtr);
        
    pstk->leftCanaryPtr = NULL;        //Высвобождаем память, занулляем указатели
    pstk->hash = NULL;
    pstk->rightCanaryPtr = NULL;
#else
    free(pstk->data);
#endif

    pstk->data = NULL;
    LOG(msgNoArgs, "STK_DESTTED", "", "");
    FILECLOSE();

    return 0;
}

int stackPush(stack *pstk, const dataType num)
{
    assert(pstk != NULL);

    int errorNum = 0;
    if (errorNum = stackVerificator(pstk))
        return errorNum;

    if (pstk->size == pstk->capacity)           //Проверка на переполнение массива
        if(stk_realloc(pstk, UP))
            return MEM_RLC_ERR;
    
    pstk->data[pstk->size] = num;
    LOG("%-25s| %-20d| %-20s|\n", "STK_PUSH", pstk->data[pstk->size], "");
    pstk->size++;

#ifdef SECURE
    *(pstk->hash) = hashFunc(pstk);
#endif
    
    return stackVerificator(pstk);
}

int stackPop(stack *pstk, dataType *num)
{    
    assert(pstk != NULL);

    int errorNum = stackVerificator(pstk);
    if (errorNum)
        return errorNum;

    if (pstk->size == 0)
    {
        LOG(msgNoArgs, ">> Stack is empty", "STK_EMPTY", "");
        return STK_EMPTY;
    }

    --pstk->size;   
    if (num != NULL)     //Попаем значение, затем заполняем его ядом
        *num = pstk->data[pstk->size];
    pstk->data[pstk->size] = poison;

#ifdef SECURE
    *(pstk->hash) = hashFunc(pstk);
#endif

    LOG(msgNoArgs, ">> Number was popped", "STK_POP", "");
    return stackVerificator(pstk);
}

int stackPrint(stack *pstk, int option)
{
    assert(pstk != NULL);
    FILE *fileName = NULL;
#ifdef DEBUG
    fileName = pstk->logFile;
    if (option)
        fileName = stderr;
#else
    fileName = stderr;
#endif
    
    int errorNum = 0;
    if (errorNum = stackVerificator(pstk))
        return errorNum;
        
    fprintf(fileName, "\n>> Stack info:\n");
    fprintf(fileName, "\n>> Size: %d", pstk->size);
    fprintf(fileName, "\n>> Capacity: %d", pstk->capacity);
    fprintf(fileName, "\n>> Current data in stack:\n");    
    for (int i = 0; i < pstk->size; i++)
        DATAPRINT(i + 1, pstk->data[i]);
#ifdef SECURE
    POISONPRINT(poison);
    CANARYPRINT(*(pstk->leftCanaryPtr), *(pstk->rightCanaryPtr));
    HASHPRINT(*(pstk->hash));
#endif

    return 0;
}

int stk_realloc(stack *pstk, const int num)
{
    assert(pstk != NULL);
    assert(num == UP || num == DOWN);

    int errorNum = 0;
    if (errorNum = stackVerificator(pstk))
        return errorNum;

    if (num == UP)
    {
        pstk->capacity *= 2;
        LOG(msgNoArgs, ">> Capacity has been doubled", "CAPACITY_UP", "");
    }
    else if(num == DOWN)
    {
        if (pstk->capacity == 1)
        {
            LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
            return MEM_RLC_ERR;
        }
        
        pstk->capacity /= 2;
        LOG(msgNoArgs, ">> Capacity has been halfed", "CAPACITY_DOWN", "");
    }
#ifdef SECURE
    canary *temp = (canary *) \
        realloc(pstk->leftCanaryPtr, pstk->capacity * sizeof(dataType) + 2 * sizeof(canary) + sizeof(hash_t));
    if (temp == NULL)
    {
        LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
        return MEM_RLC_ERR;
    }        

    pstk->leftCanaryPtr = temp;
    pstk->hash = (hash_t *)(temp + 1);
    pstk->data = (dataType *)(pstk->hash + 1);             //Присваиваем указатели данным, хешу и канарейкам

    pstk->rightCanaryPtr = (canary *)(pstk->data + pstk->capacity);
    *(pstk->rightCanaryPtr) = Rcanary;      //Присваиваем значение новой конечной канарейке, значение старой сотрётся ядом
#else
    dataType *temp = (dataType *)realloc(pstk->data, pstk->capacity * sizeof(dataType));
    if (temp == NULL)
    {
        LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
        return MEM_RLC_ERR;
    }

    pstk->data = temp;
#endif

    if (num == UP)
        if (poisonFunc(pstk))
        return PSN_ERR;

    return stackVerificator(pstk);
}

hash_t hashFunc(const stack *pstk)
{
    hash_t hashHolder = 0;
    for (int i = 0; i < pstk->size; i++)
        hashHolder += pstk->data[i] * pstk->size;
    hashHolder += pstk->capacity;
    hashHolder += pstk->size;
    
    return hashHolder;
}

int poisonFunc(stack *pstk)
{
    if (pstk == NULL)
    {
        LOG(msgNoArgs, ">> Poison error: given a NULL pointer", "NULL_PTR", "[error]");
        return 1;
    }
    
    for (int i = pstk->size; i < pstk->capacity ; i++)
        pstk->data[i] = poison;

    return 0;
}

int poisonCheck(const stack *pstk)
{
    for (int pos = pstk->size; pos < pstk->capacity; pos++)
        if (pstk->data[pos] != poison)
            return PSN_ERR;
  
    return 0;
}

int canaryCheck(const stack *pstk)
{
    if (*(pstk->leftCanaryPtr) != Lcanary || *(pstk->rightCanaryPtr) != Rcanary)
        return CANARY_ERR;
        
    return 0;
}

int hashCheck(const stack *pstk)
{
    if (*(pstk->hash) != hashFunc(pstk))
        return HASH_ERR;

    return 0;
}

int stackVerificator(const stack *pstk)
{
    if (pstk == NULL)
    {
        LOG(msgNoArgs, ">> Given a NULL pointer", "NULL_ERR", "[error]", "");
        return NULL_PTR;
    }
    if (pstk->capacity == 0)
    {
        LOG(msgNoArgs, ">> Capacity can't be zero", "CAPACITY_ZERO", "[error]", "");
        return CAPACITY_ZERO;
    }
    if (poisonCheck(pstk))
    {
        LOG(msgNoArgs, ">> Number is not poison", "PSN_ERR", "[error]", "");
        return PSN_ERR;
    }
    if (canaryCheck(pstk))
    {
        LOG(msgNoArgs, ">> Canary is damaged. ", "CANARY_ERR", "[error]", "");
        return CANARY_ERR;
    }
    if (hashCheck(pstk))
    {
        LOG(msgNoArgs, ">> Hash is not matching.", "HASH_ERR", "[danger]", "");
        return HASH_ERR;
    }
    
    return 0;
}

void stackFileLog(FILE *fileName, const char *format, ...)
{  
    va_list arg = {};   

    va_start(arg, format);
    vfprintf(fileName, format, arg);
    va_end(arg);

    fflush(fileName);
}
//printf(различных типов данных)