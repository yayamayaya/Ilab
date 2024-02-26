#pragma once

typedef int treeDataType;
#define TREE_DATA_SPEC 

typedef struct node
{
    treeDataType data;
    struct node *left;
    struct node *right;
} Node_t;

int nodeInsert(Node_t *node, treeDataType data);
int treePrint(Node_t *node);
int treeIncrementPrint(Node_t *node);
int treeKill(Node_t *node);

