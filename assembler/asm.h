#pragma once

#define COMMANDS_NUM 20
#define REGISTERS_NUM 4

#define REGISTER_POSITION_AS_ARGUMENT 2

typedef unsigned char byte_t;

typedef double data_t;
#define DATA_SPEC "%lf%n"

enum ARGUMENT_TYPE
{
    ARG_NUMBER = 1,
    ARG_RAM = 2,
    ARG_REGISTER = 3
};

enum ASM
{
    PUSH_GRP        = 0x81,    

    ADD             = 0x02,     //Сложение
    SUB             = 0x03,     //Вычитание
    MULT            = 0x04,     //Умножение
    DIV             = 0x05,     //Деление
    SQRT            = 0x0A,     //Квадратный корень

    POP_GRP         = 0x82,
    
    IN              = 0x06,     //Ввод с консоли   

    MOV_GRP         = 0x93,     //Команда mov

    OUT             = 0x07,     //Вывод

    HALT            = 0x0F,     //Завершение 

    NOCMD           = 0xF0,

    //GRAPHICS        = ,    

    CALL            = 0x84,     //"Вызов"
    
    JMP             = 0x86,     //Переход на какую то позицию
    JB              = 0x87,     //Условные переходы
    JBE             = 0x88,
    JA              = 0x89,
    JAE             = 0x8A,
    JE              = 0x8B,
    JNE             = 0x8C,

    RET             = 0x09,     //Возвращение

    //Коды аргументов(4 бита):
    NUM_ARG         = 0b0001,
    REG_ARG         = 0b0010,
    RAM_ARG         = 0b0100,
    ADR_ARG         = 0b1000,
    RAMREG_ARG      = 0b0110,
    REGNUM_ARG      = 0b0011,
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
    ASM_MEM_ALC_ERR = 101,
    COMP_ERR = 404,
    NOLABEL = 0xF0
};

typedef struct
{
    const char *commandName;
    enum ASM commandNum;
} cmd;

cmd commands[COMMANDS_NUM] = {
{"push", PUSH_GRP},
{"add", ADD},
{"sub", SUB},
{"mult", MULT},
{"div", DIV},
{"ret", RET},
{"out", OUT},
{"halt", HALT},
{"in", IN},
{"pop", POP_GRP},
{"jmp", JMP},
{"jb", JB},
{"jbe", JBE},
{"ja", JA},
{"jae", JAE},
{"je", JE},
{"jne", JNE},
{"call", CALL},
{"mov", MOV_GRP},
{"sqrt", SQRT},
//{"graph", GRAPHICS}
};

/*cmd registers[REGISTERS_NUM] = {
{"ax", AX},
{"bx", BX},
{"cx", CX},
{"dx", DX}
};*/

/*
Обработка аргументов:

push [rax] 0000 0110
push [rax + 5] 0000 0111        [rax\0+\05]
push [5] 0000 0101
push rax 0000 0100
push 5 0000 0001


mov [rax + 5],[bax + 1] 0101 1111 0101 0111
*/