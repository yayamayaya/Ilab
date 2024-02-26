#pragma once

#define COMMANDS_NUM 20
#define REGISTERS_NUM 4

typedef unsigned char byte_t;

enum ARGUMENT_TYPE
{
    ARG_NUMBER = 1,
    ARG_RAM = 2,
    ARG_REGISTER = 3
};

enum ASM
{
    PUSH_GRP = 0,    
    PUSH = 0x0B, //Число в стек
    RPUSH = 0x0C,  //Число из регистра в стек
    RAMPUSH = 0x0D,

    ARITHM_GRP = 0x10,
    ADD = 0x11,   //Сложение
    SUB = 0x12,   //Вычитание
    MULT = 0x13,  //Умножение
    DIV = 0x14, //Деление

    POP_GRP = 0x20,
    RPOP = 0x21,   //Число из стек в регистр   
    RAMPOP = 0x22,
    
    IN = 0x30,    //Ввод с консоли   

    MOV_GRP = 0x40,     //Команда mov
    RAM_FRST = 0b1000,   
    REG_FRST = 0b0100,
    NUM_SCND = 0b0011,
    RAM_SCND = 0b0010,
    REG_SCND = 0b0001,

    OUT = 0x50,   //Вывод

    HALT = 0x60,  //Завершение 

    GRAPHICS = 0x70,    

    CALL = 0x80, //"Вызов"

    JMP_GRP = 0x90,
    JMP = 0x91,    //Переход на какую то позицию
    JB = 0x92,    //Условные переходы
    JBE = 0x93,
    JA = 0x94,
    JAE = 0x95,
    JE = 0x96,
    JNE = 0x97,

    RET = 0xA0,   //Возвращение

    NOCMD = 0xF0,
    //Регистры:
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4
};

enum numberTypes
{
    ADDRESS = 1,
    DATA = 2
};

enum asmDebug
{
    FILEREAD_ERR = 501,
    FILENULL_ERR,
    INST_DET_ERR,
    SIZEOFNUM_ERR,
    ARG_DET_ERR,
    FILEWRITE_ERR,
    FILEOPEN_ERR,
    FATAL_ERR,
    COMP_ERR = 404,
};

typedef struct
{
    const char *commandName;
    enum ASM commandNum;
} cmd;

cmd commands[COMMANDS_NUM] = {
{"push", PUSH},
{"add", ADD},
{"sub", SUB},
{"mult", MULT},
{"div", DIV},
{"ret", RET},
{"out", OUT},
{"halt", HALT},
{"in", IN},
{"pop", RPOP},
{"jmp", JMP},
{"jb", JB},
{"jbe", JBE},
{"ja", JA},
{"jae", JAE},
{"je", JE},
{"jne", JNE},
{"call", CALL},
{"mov", MOV_GRP},
{"graph", GRAPHICS}
};

cmd registers[REGISTERS_NUM] = {
{"ax", AX},
{"bx", BX},
{"cx", CX},
{"dx", DX}
};