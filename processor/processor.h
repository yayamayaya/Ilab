#pragma once

#include "../stack/stack.h"
#include "../assembler/asm.h"
#include "../onegin/fileReader.h"
#include "../common/log.h"

enum DEBUG_CODE
{
    FATAL_ERROR = 1001,
    PROGRAMM_ENDED = 1,
    ARITHM_GRP_ERR = 100,
    PUSH_CMD_FAIL = 10,
    RPUSH_CMD_FAIL,
    RAMPUSH_CMD_FAIL,
    RPOP_CMD_FAIL,
    RAMPOP_CMD_FAIL,
    IN_CMD__SCAN_ERR,
    IN_CMD__PUSH_FAIL,
    MOV_CMD_FAIL,
    CALL_CMD__PUSH_FAIL,
    RET_CMD_FAIL,
    ARITHM_CMD_FAIL,
    ARG_TAKE_FAIL
};

enum BITMASKS
{
    MOST_SIGN_MASK = 0xF0,
    LESS_SIGN_MASK = 0x0F
};