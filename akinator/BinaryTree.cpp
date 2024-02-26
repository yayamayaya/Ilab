#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include "BinaryTree.h"

FILE *logFile = NULL;

static void fileLog(const char *format, ...);

int nodeInsert(Node_t *node, treeDataType data)
{
    assert(node != NULL);

    Node_t *tempNode = (Node_t *)calloc(1, sizeof(Node_t));
    tempNode->data = data;

    while(1)
        if (tempNode->data < node->data)
            if (node->left != NULL)
                node = node->left;
            else
            {
                node->left = tempNode;
                break;
            }
        else if (tempNode->data > node->data)
            if (node->right != NULL)
                node = node->right;
            else
            {
                node->right = tempNode;
                break;
            }

    return 0;
}

int treePrint(Node_t *node)
{
    fprintf(stderr, "{%d", node->data);
    if (node->left != NULL)
        treePrint(node->left);
    if (node->right != NULL)
        treePrint(node->right);

    fprintf(stderr, "}");

    return 0;
}

int treeIncrementPrint(Node_t *node)
{
    assert(node != NULL);

    if (node->left != NULL)
        treeIncrementPrint(node->left);

    fprintf(stderr, "%d -> ", node->data);

    if (node->right != NULL)
        treeIncrementPrint(node->right);

    return 0;
}

int treeKill(Node_t *node)
{
    assert(node != NULL);

    if (node->left != NULL)
        treeKill(node->left);
    if (node->right != NULL)
        treeKill(node->right);

    free(node);

    return 0;
}

//Сделать поиск и удаление одного элемента


static void fileLog(const char *format, ...)
{
    va_list args = {};

    va_start(args, format);
    fprintf(logFile, format, args);
    va_end(args);

    fflush(logFile);
}