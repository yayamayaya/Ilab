#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "akinator.h"

int takeDescription(class stack<char> *description, node_t *node, char *objName)
{
    assert(node);
    assert(objName);

    if (!node->left && !node->right)
        if (!strcmp(node->data, objName))
        {
             //printf(">>> object found\n");
            return 1;            
        }
        else
            return 0;
    
    node_t *foundedNode = NULL;
        
    if (node->left)
    {
        //printf(">> going to the left node\n\n");
        description->stackPush(1);
        if (takeDescription(description, node->left, objName))
            return 1; 
        //printf(">> node in the left not found, going next\n\n");
        description->stackPop(NULL);    
    }
    if (node->right)
    {
        //printf(">> going to the right node\n\n");
        description->stackPush(0);
        if (takeDescription(description, node->right, objName))
            return 1;  
        //printf(">> node in the right not found, going next\n\n");
        description->stackPop(NULL);            
    }

    return 0;
}

class stack<char> giveDescription(node_t *root, const char *descrLogName)
{
    assert(root);

    char *object = NULL;
    takeAString(&object);

    stack<char> description = {};
    description.stackCtor(10, descrLogName);
    takeDescription(&description, root, object);
    description.stackPrint(IN_CONSOLE);
    free(object);

    return description;
}

int compareTwoObjects(node_t *root)
{
    assert(root);

    printf("Give a name of an object, which you want to compare:\n");
    stack<char> description1 = giveDescription(root, "descriptionLog1.log");
    printf("Give a name of another object:\n");
    stack<char> description2 = giveDescription(root, "descriptionLog2.log");
    description1.stackDtor();
    description2.stackDtor();

    return 0;
}