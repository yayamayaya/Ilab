#pragma once

#define MAX_STRING_LENGTH 50

enum ANSWERS 
{
    YES = 1,
    NO = 0
};

enum AKINATOR_RESULTS
{
    ANSWER_FOUND                = 0,
    DESCRIPTION_ADDED           = 0,
    ANSWER_ADDED                = 0,
};

enum ERRORS
{
    NEW_DESCRIPTION_ERR         = 101,
    MEM_ALC_ERR,
    STRING_TAKING_ERR,
    YES_OR_NO_ERR               = 3,
    NULL_STR                    = 125,
};