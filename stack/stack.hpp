#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../common/log.h"
#include "stack.h"


#ifndef STACK
#define STACK

#ifdef DEBUG
#define LOG(...) fileLog(this->logFile, __VA_ARGS__) 
#define FILECLOSE()               \
    fclose(this->logFile);        \
    this->logFile = NULL
#else
#define LOG(...)
#define FILECLOSE()
#endif

const char *msgNoArgs = "%-25s| %-20s| %-20s|\n";
const char *msgOneArg = "%-25s| %-20d| %-20s|\n";

//const dataType poison = 0xDD;
const canary Lcanary = 0xDEDDEAD;
const canary Rcanary = 0xFFABEBA;

template <typename T>
int stack<T>::stackCtor(const int capacity, const char* logFileName)
{
    //assert(this != NULL);
    assert(capacity != 0);

#ifdef DEBUG            //Открываем логфайл
    assert(logFileName != NULL);
    this->logFile = fopen(logFileName, "w");
    if (this->logFile == NULL)  //Проверка на нулевой указатель
    {
        printf("[error]>>Can't open stack log.\n");
        return LOG_FILE_OPEN_ERR;
        //#undef DEBUG        
    }
    fileLog(this->logFile, "%-25s| %-20s| %-20s|\n\n", "INSTRUCT. NAME", "ARGUMENTS", "ERROR");
#endif

#ifdef SECURE
    this->leftCanaryPtr = (canary *)calloc(capacity * sizeof(T) + 2 * sizeof(canary) + sizeof(hash_t), 1);       
    if (this->leftCanaryPtr == NULL)        //Аллоцируем память под канареек, хеш и данные, присваиваем адреса памяти канарейкам и данным
    {
        LOG(msgNoArgs, "MEM_ALC_ERR", "", "[error]");
#ifdef DEBUG
        fclose(this->logFile);
#endif
        return MEM_ALC_ERR;
    }

    this->hash = (hash_t *)(this->leftCanaryPtr + 1);
    this->data_ = (T *)(this->hash + 1);
    this->rightCanaryPtr = (canary *)(this->data_ + capacity);

    *(this->leftCanaryPtr) = Lcanary;   //Присваиваем канарейкам значения
    *(this->rightCanaryPtr) = Rcanary; 
#else
    this->data_ = (T *)calloc(capacity, sizeof(T));
    if(this->data_ == NULL)
    {
        LOG(msgNoArgs, "MEM_ALC_ERR", "", "[error]");
#ifdef DEBUG
        fclose(this->logFile);
#endif
        return MEM_ALC_ERR;
    }
#endif

    this->size = 0;
    this->capacity = capacity;

    if (poisonFunc())
        return PSN_ERR;        

    *(this->hash) = hashFunc();   

    LOG(msgNoArgs, "STK_CONSTED", "", ""); 
    return stackVerificator();
}

template <typename T>
int stack<T>::stackDtor()
{
    //assert(this != NULL);

    int errorNum = stackVerificator();
    
    this->capacity = 0;
    this->size = 0;

#ifdef SECURE
    free(this->leftCanaryPtr);
        
    this->leftCanaryPtr = NULL;        //Высвобождаем память, занулляем указатели
    this->hash = NULL;
    this->rightCanaryPtr = NULL;
#else
    free(this->data_);
#endif

    this->data_ = NULL;
    LOG(msgNoArgs, "STK_DESTTED", "", "");
    FILECLOSE();

    return 0;
}

template <typename T>
int stack<T>::stackPush(const T num)
{
    assert(this != NULL);

    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (this->size == this->capacity)           //Проверка на переполнение массива
        if(stk_realloc(UP))
            return MEM_RLC_ERR;
    
    this->data_[this->size] = num;
    LOG("%-25s| %-20lld| %-20s|\n", "STK_PUSH", this->data_[this->size], "");
    this->size++;

#ifdef SECURE
    *(this->hash) = hashFunc();
#endif
    
    return stackVerificator();
}

template <typename T>
int stack<T>::stackPop(T *num)
{    
    assert(this != NULL);

    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (this->size == 0)
    {
        LOG(msgNoArgs, ">> Stack is empty", "STK_EMPTY", "");
        return STK_EMPTY;
    }

    --this->size;   
    if (num != NULL)     //Попаем значение, затем заполняем его ядом
        *num = this->data_[this->size];
    this->data_[this->size] = poison;

#ifdef SECURE
    *(this->hash) = hashFunc();
#endif

    LOG(msgNoArgs, ">> Number was popped", "STK_POP", "");
    return stackVerificator();
}

template <typename T>
int stack<T>::stackPrint(int option)
{
    assert(this != NULL);
    FILE *fileName = NULL;
#ifdef DEBUG
    fileName = this->logFile;
    if (option)
        fileName = stderr;
#else
    fileName = stderr;
#endif
    
    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;
        
    fprintf(fileName, "\n>> Stack info:\n");
    fprintf(fileName, "\n>> Size: %d", this->size);
    fprintf(fileName, "\n>> Capacity: %d", this->capacity);
    fprintf(fileName, "\n>> Current data_ in stack:\n");    
    for (int i = 0; i < this->size; i++)
    {
        fprintf(fileName, "%d) ", i + 1);
        data_print(fileName, this->data_[i]);        
    }

#ifdef SECURE
    fprintf(fileName, "Poison is: ");
    data_print(fileName, poison);
    CANARYPRINT(*(this->leftCanaryPtr), *(this->rightCanaryPtr));
    HASHPRINT(*(this->hash));
#endif

    return 0;
}

template <typename T>
int stack<T>::stk_realloc(const int num)
{
    assert(this != NULL);
    assert(num == UP || num == DOWN);

    int errorNum = stackVerificator();
    if (errorNum)
        return errorNum;

    if (num == UP)
    {
        this->capacity *= 2;
        LOG(msgNoArgs, ">> Capacity doubled", "CAPACITY_UP", "");
    }
    else if(num == DOWN)
    {
        if (this->capacity == 1)
        {
            LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
            return MEM_RLC_ERR;
        }
        
        this->capacity /= 2;
        LOG(msgNoArgs, ">> Capacity halfed", "CAPACITY_DOWN", "");
    }
#ifdef SECURE
    canary *temp = (canary *) \
        realloc(this->leftCanaryPtr, this->capacity * sizeof(T) + 2 * sizeof(canary) + sizeof(hash_t));
    if (temp == NULL)
    {
        LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
        return MEM_RLC_ERR;
    }        

    this->leftCanaryPtr = temp;
    this->hash = (hash_t *)(temp + 1);
    this->data_ = (T *)(this->hash + 1);             //Присваиваем указатели данным, хешу и канарейкам

    this->rightCanaryPtr = (canary *)(this->data_ + this->capacity);
    *(this->rightCanaryPtr) = Rcanary;      //Присваиваем значение новой конечной канарейке, значение старой сотрётся ядом
#else
    T *temp = (T *)realloc(this->data_, this->capacity * sizeof(T));
    if (temp == NULL)
    {
        LOG(msgNoArgs, ">> Memory reallocatiom error", "MEM_RLC_ERR", "[error]");
        return MEM_RLC_ERR;
    }

    this->data_ = temp;
#endif

    if (num == UP)
        if (poisonFunc())
        return PSN_ERR;
    
#ifdef SECURE
    *(this->hash) = hashFunc();
#endif

    return stackVerificator();
}

template <typename T>
hash_t stack<T>::hashFunc()
{
    hash_t hashHolder = 0;
    for (int i = 0; i < this->size; i++)
        hashHolder += this->data_[i] * this->size;
    hashHolder += this->capacity;
    hashHolder += this->size;
    
    return hashHolder;
}

template <typename T>
int stack<T>::poisonFunc()
{
    if (this == NULL)
    {
        LOG(msgNoArgs, ">> Poison error: given a NULL pointer", "NULL_PTR", "[error]");
        return 1;
    }
    
    for (int i = this->size; i < this->capacity ; i++)
        this->data_[i] = poison;

    return 0;
}

template <typename T>
int stack<T>::poisonCheck()
{
    for (int pos = this->size; pos < this->capacity; pos++)
        if (this->data_[pos] != poison)
            return PSN_ERR;
  
    return 0;
}

template <typename T>
int stack<T>::canaryCheck()
{
    if (*(this->leftCanaryPtr) != Lcanary || *(this->rightCanaryPtr) != Rcanary)
        return CANARY_ERR;
        
    return 0;
}

template <typename T>
int stack<T>::hashCheck()
{
    if (*(this->hash) != hashFunc())
        return HASH_ERR;

    return 0;
}

template <typename T>
int stack<T>::stackVerificator()
{
    if (this == NULL)
    {
        LOG(msgNoArgs, ">> Given a NULL pointer", "NULL_ERR", "[error]", "");
        return NULL_PTR;
    }
    if (this->capacity == 0)
    {
        LOG(msgNoArgs, ">> Capacity can't be zero", "CAPACITY_ZERO", "[error]", "");
        return CAPACITY_ZERO;
    }
    if (poisonCheck())
    {
        LOG(msgNoArgs, ">> Number is not poison", "PSN_ERR", "[error]", "");
        return PSN_ERR;
    }
    if (canaryCheck())
    {
        LOG(msgNoArgs, ">> Canary is damaged. ", "CANARY_ERR", "[error]", "");
        return CANARY_ERR;
    }
    if (hashCheck())
    {
        LOG(msgNoArgs, ">> Hash is not matching.", "HASH_ERR", "[danger]", "");
        return HASH_ERR;
    }
    
    return 0;
}

void data_print(FILE *fileName, const int num)
{
    fprintf(fileName, "%d\n", num);
}

void data_print(FILE *fileName, const double num)
{
    fprintf(fileName, "%.2f\n", num);
}

void data_print(FILE *fileName, const long long int num)
{
    fprintf(fileName, "%lld\n", num);
}


#endif