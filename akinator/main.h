#ifndef AKINATOR
#define AKINATOR

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "../common/log.h"
#include "../binary_tree/BinaryTree.h"
#include "../stack/stack.h"
#include "../onegin/fileReader.h"
#include "data/dataBaseRead.h"

#define DATABASE_LOC "../akinator/data/dataBase.txt"

#define MAX_STRING_LENGTH 50

#ifdef DEBUG
#define LOG(format, ...) fileLog(akLog, format, __VA_ARGS__)
#else
#define LOG(format, ...) do {} while (0)
#endif

int takeAString(char **strPtr);
int akinatorAsk(const char *format, const char *string);
int addDescription(node_t *node);
void addAnswer(node_t *node);
int Akinator(node_t *treeNode);

int findDescription(class Stack<char> *description, node_t *node, char *objName);
class Stack<char> takeDescription(node_t *root, const char *descrLogName);
void giveDescription(node_t *node);
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