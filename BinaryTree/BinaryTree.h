#pragma once

typedef int treeDataType;
#define TREE_DATA_SPEC 

typedef struct node
{
    treeDataType data;
    struct node *left;
    struct node *right;
} node_t;

node_t *nodeInsert(node_t *node, treeDataType data);
void treePrint(FILE *fileName, node_t *node);
int treeIncrementPrint(FILE *fileName, node_t *node);
int treeKill_static(node_t *node);
int treeKill_string(node_t *node);
//int treeKill(node_t *node);
node_t *findNode(node_t *nodePtr, const int num);
void deleteNode(node_t *treePtr, const treeDataType num);
