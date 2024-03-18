#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "akinator.h"

int findDescription(class Stack<char> *description, node_t *node, char *objName)
{
    assert(node);
    assert(objName);

    if (!node->left && !node->right)
        return !strcmp(node->data, objName);
        
    node_t *foundedNode = NULL;
        
    if (node->left)
    {
        //printf(">>  >> going to the left node\n\n");
        description->stackPush(1);
        if (findDescription(description, node->left, objName))
            return 1; 
        //printf(">>  >> node in the left not found, going next\n\n");
        description->stackPop(NULL);    
    }
    if (node->right)
    {
        //printf(">>  >> going to the right node\n\n");
        description->stackPush(0);
        if (findDescription(description, node->right, objName))
            return 1;  
        //printf(">>  >> node in the right not found, going next\n\n");
        description->stackPop(NULL);            
    }

    return 0;
}

class Stack<char> takeDescription(node_t *root, const char *descrLogName)
{
    assert(root);

    char *object = NULL;
    takeAString(&object);

    Stack<char> description = {};
    description.stackCtor(10, descrLogName);
    findDescription(&description, root, object);
    //description.stackPrint(IN_CONSOLE);
    free(object);

    if (description.size == 0)
        printf(">>  Given object not found.\n");
    

    return description;
}

void giveDescription(node_t *node)
{
    printf(">>  Give a name of an object, which description you want to find:\n");
    Stack<char> description = takeDescription(node, "descriptionLog.log");

    if (description.size == 0)
    {
        description.stackDtor();
        return;
    }
    
    printf(">>  Given object has next description:\n");
    for (int i = 0; i < description.size; i++)
    {
        if(node == NULL)
            break;

        if (description.getDataOnPos(i))
        {
            printf("%s", node->data);
            node = node->left;
        }
        else
        {   
            printf("not %s", node->data);
            node = node->right;
        }

        if (i == description.size - 1)
            printf(".\n", node->data);     
        else 
            printf(", ");       

    }   
    
    description.stackDtor();
    return;
}

int compareTwoObjects(node_t *node)
{
    assert(node);

    printf(">>  Give a name of an object, which you want to compare:\n");
    Stack<char> description1 = takeDescription(node, "descriptionLog1.log");
    printf(">>  Give a name of another object:\n");
    Stack<char> description2 = takeDescription(node, "descriptionLog2.log");

    printf(">> These objects are:\n");

    for (int i = 0; i < description1.size && i < description2.size; i++)
    {
        if (description1.getDataOnPos(i) == description2.getDataOnPos(i))
            if (description1.getDataOnPos(i))
            {
                printf("both %s", node->data);
                node = node->left;
                continue;
            }
            else
            {   
                printf("both not %s", node->data);
                node = node->right;
                continue;
            }
        
    }
    
    
    
    description1.stackDtor();
    description2.stackDtor();

    return 0;
}
