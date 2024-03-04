#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include "BinaryTree.h"

FILE *logFile = NULL;

static void fileLog(const char *format, ...);

Node_t *nodeInsert(Node_t *node, const treeDataType data)
{
    assert(node != NULL);

    Node_t *tempNode = (Node_t *)calloc(1, sizeof(Node_t));
    tempNode->data = data;

    while(1)
        if (tempNode->data <= node->data)
            if (node->left != NULL)
                node = node->left;
            else
            {
                node->left = tempNode;
                return node->left;
            }
        else if (tempNode->data >= node->data)
            if (node->right != NULL)
                node = node->right;
            else
            {
                node->right = tempNode;
                return node->right;
            }

    return NULL;
}

Node_t *findNode(Node_t *treePtr, const treeDataType num)
{
    assert(treePtr);

    while(1)
        if (num < treePtr->data)
            if (treePtr->left != NULL)
                treePtr = treePtr->left;
            else
                return NULL; 
        else if (num > treePtr->data)
            if (treePtr->right != NULL)
                treePtr = treePtr->right;
            else
                return NULL; 
        else if (num == treePtr->data)
            return treePtr;

    return NULL;
}

int deleteNodePtr(Node_t *treePtr, const Node_t *Ptr)
{
    assert(treePtr);
    assert(Ptr);
    
    if (treePtr->left != NULL && treePtr->left != Ptr)
        deleteNodePtr(treePtr->left, Ptr);
    if (treePtr->right != NULL && treePtr->right != Ptr)
        deleteNodePtr(treePtr->right, Ptr);

    if (treePtr->left == Ptr)
    {
        treePtr->left = NULL;
        return 0;
    }
    if (treePtr->right == Ptr)
    {
        treePtr->right = NULL;
        return 0;
    }

    return 1;
}

void deleteNode(Node_t *treePtr, const treeDataType num)
{
    assert(treePtr);

    Node_t *nodeToChange = findNode(treePtr, num);
    if (!nodeToChange)
    {
        //LOG
        return;
    }

    Node_t *killedNodeLeftT = nodeToChange->left;
    Node_t *killedNodeRightT = nodeToChange->right;
    
    if (killedNodeRightT == NULL && killedNodeLeftT == NULL)
    {
        if (deleteNodePtr(treePtr, nodeToChange))
        {
            return;
        }
        free(nodeToChange);
        return;
    }
    else if (killedNodeRightT == NULL && killedNodeLeftT != NULL)
    {
        nodeToChange->data = killedNodeLeftT->data;
        nodeToChange->left = killedNodeLeftT->left;

        if (deleteNodePtr(treePtr, killedNodeLeftT))
        {
            return;
        }

        free(killedNodeLeftT);

        return;
        
    }   
    else if (killedNodeRightT != NULL && killedNodeLeftT == NULL)
    {
        nodeToChange->data = killedNodeRightT->data;
        nodeToChange->right = killedNodeRightT->right;

        if (deleteNodePtr(treePtr, killedNodeRightT))
        {
            return;
        }

        free(killedNodeRightT);

        return;
    }

    nodeToChange->data = killedNodeRightT->data;
    nodeToChange->right = killedNodeRightT->right;
    nodeToChange->left = killedNodeRightT->left;

    free(killedNodeRightT);

    Node_t *newNode = nodeInsert(treePtr, killedNodeLeftT->data);
    if (!newNode)
    {
        //error
        return;
    }

    newNode->left = killedNodeLeftT->left;
    newNode->right = killedNodeLeftT->right;

    return;    
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

static void fileLog(const char *format, ...)
{
    va_list args = {};

    va_start(args, format);
    fprintf(logFile, format, args);
    va_end(args);

    fflush(logFile);
}