#pragma once
#include <stdio.h>

#ifndef STACK_HEADER
#define STACK_HEADER

#define CANARYPRINT(arg1, arg2) fprintf(fileName, ">> Canaries are %llX & %llX\n", arg1, arg2)
#define HASHPRINT(arg1) fprintf(fileName, ">> Hash is: %llX\n", arg1)

typedef long long int canary_t;    
typedef long long int hash_t;


template <typename T>
class Stack 
{
public:

    int stackCtor(const int stackCapacity, const char* logFileName);
    int stackDtor();
    int stackPush(const T num);
    int stackPop(T *num);
    int stackPrint(int option);
    int stackVerificator();
    int stk_realloc(const int num);
    int getDataOnPos(const int pos);

private:
    static constexpr canary_t kLeftCanary = 0xABEBADED;
    static constexpr canary_t kRightCanary = 0xDEDABEBA;

    static constexpr canary_t Lcanary = 0xDEDDEAD;
    static constexpr canary_t Rcanary = 0xFFABEBA;


    static const T poison;

    hash_t hashFunc();
    int poisonFunc();
    int poisonCheck();
    int canaryCheck();
    int hashCheck();

    canary_t *getLeftCanaryPtr();
    hash_t *getHashNumberPtr();
    T *getDataPtr();
    canary_t *getRightCanaryPtr();

    
#ifdef DEBUG
    FILE *logFile;
#endif

    canary_t classLCanary = kLeftCanary;

    T* data;
    int size;
    int capacity;

    canary_t classRCanary = kRightCanary;
};

enum debugging
{
    STK_CTED = 0,
    CAPACITY_ZERO = 1,
    STK_PUSH = 4,
    STK_POP = 5,
    STK_EMPTY = 6,
    CAPACITY_UP = 7,
    CAPACITY_DOWN = 8,
    STK_DTED = 9,
    STK_KILL = -1
};

enum errors
{
    STK_MEM_ALC_ERR = 515,
    STK_MEM_RLC_ERR = 516,
    PSN_ERR = 10,
    CANARY_ERR = 11,
    HASH_ERR = 12,
    NULL_PTR = 13,
    LOG_OPEN_ERR = 20

};

enum stk_realloc
{
    DOWN = 0, 
    UP = 1
};

enum STK_PRINT_OPTIONS
{
    IN_FILE = 0,
    IN_CONSOLE = 1
};

#include "stack.hpp"
#endif