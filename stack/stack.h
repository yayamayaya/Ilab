#pragma once
#include <stdio.h>

#ifndef DEBUG
    #define DEBUG
#endif
#define SECURE
#define KILL 

typedef int dataType;
#define DATAPRINT(arg1, arg2) fprintf(fileName, "%d) %d\n", arg1, arg2)
#define POISONPRINT(arg) fprintf(fileName, ">> Poison is: %X\n", arg)
typedef long long int canary;
#define CANARYPRINT(arg1, arg2) fprintf(fileName, ">> Canaries are %llX & %llX\n", arg1, arg2)
typedef long long int hash_t;
#define HASHPRINT(arg1) fprintf(fileName, ">> Hash is: %llX\n", arg1)

typedef struct 
{
    dataType* data;
    int size;
    int capacity;
#ifdef SECURE
    canary *leftCanaryPtr;
    hash_t *hash;                    //hash - число, высчитываемое из данных в стеке
    canary *rightCanaryPtr;
#endif
#ifdef DEBUG
    FILE *logFile;
#endif
} stack;

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
    MEM_ALC_ERR = 515,
    MEM_RLC_ERR = 516,
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

int stackCtor(stack *pstk, const int capacity, const char* logFileName);
int stackDtor(stack *pstk);
int stackPush(stack *pstk, const dataType num);
int stackPop(stack *pstk, dataType *num);
int stackPrint(stack *pstk, int option);
int stk_realloc(stack *stk, const int num);
//void stackFileLog(FILE *fileName, char *format, ...); __attribute__ ((format(printf, 2, 3)));