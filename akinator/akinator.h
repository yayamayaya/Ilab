#pragma once

#ifndef AKINATOR
#define AKINATOR

#include <assert.h>
#include "../common/log.h"
#include "../BinaryTree/BinaryTree.h"
#include "../stack/stack.hpp"

#define MAX_STRING_LENGTH 50

int takeAString(char **strPtr);
int akinatorAsk(const char *format, const char *string);
int addDescription(node_t *node);
void addAnswer(node_t *node);
int Akinator(node_t *treeNode);

int takeDescription(class stack<char> *description, node_t *node, char *objName);
class stack<char> giveDescription(node_t *root, const char *descrLogName);
int compareTwoObjects(node_t *root);

enum ANSWERS 
{
    YES = 1,
    NO = 0
};

enum PROG_CHOOSE_OPTION
{
    END_PROGRAM     = 0,
    AKINATE         = 1,
    DESCRIPTION     = 2,
    COMPARE         = 3,
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
    FATAL_ERR,
    YES_OR_NO_ERR               = 3,
    NULL_STR                    = 125,
};

#endif