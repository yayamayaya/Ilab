#include <stdio.h>

#ifndef STACK_HEADER
#define STACK_HEADER

#define SECURE

typedef double dataType;
/*#define DATAPRINT(arg1, arg2) fprintf(fileName, "%d) %d\n", arg1, arg2)
#define POISONPRINT(arg) fprintf(fileName, ">> Poison is: %X\n", arg)*/
#define DATA_SPEC "%lf%n"

typedef long long int canary;
#define CANARYPRINT(arg1, arg2) fprintf(fileName, ">> Canaries are %llX & %llX\n", arg1, arg2)

typedef long long int hash_t;
#define HASHPRINT(arg1) fprintf(fileName, ">> Hash is: %llX\n", arg1)

template <typename T>
struct stack 
{
    T* data_;
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


    int stk_realloc(const int num);
    hash_t hashFunc();
    int poisonFunc();
    int poisonCheck();
    int canaryCheck();
    int hashCheck();
    int stackCtor(const int capacity, const char* logFileName);
    int stackDtor();
    int stackPush(const T num);
    int stackPop(T *num);
    int stackPrint(int option);
    int stackVerificator();

    const T poison = 0xDD;
};

//Исправить
//const int stack<int>::poison = 0xDD;

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

//Программа стека:
#include "stack.hpp"

#endif