#include <string.h>
#include "../main.h"

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

    if (description.getStackSize() == 0)
        printf(">>  Given object not found.\n");
    

    return description;
}

void giveDescription(node_t *node)
{
    printf(">>  Give a name of an object, which description you want to find:\n");
    Stack<char> description = takeDescription(node, "descriptionLog.log");

    int descrSize = description.getStackSize();
    if (!descrSize)
    {
        description.stackDtor();
        return;
    }
    
    printf(">>  Given object has next description:\n");
    for (int i = 0; i < descrSize; i++)
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

        if (i == descrSize - 1)
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

    node_t *node1 = node;
    node_t *node2 = node;

    int descr1Size = description1.getStackSize();
    int descr2Size = description2.getStackSize();

    printf(">> These objects are:\n");
    int pos = 0;
    
        if (description1.getDataOnPos(0) == description2.getDataOnPos(0))
            printf(">> Both objects are: ");

        while (description1.getDataOnPos(pos) == description2.getDataOnPos(pos))
        {
            if (description1.getDataOnPos(pos))
            {
                printf(" %s,", node1->data);
                node1 = node1->left;
                node2 = node2->left;
            }
            else
            {   
                printf("both not %s", node1->data);
                node1 = node1->left;
                node2 = node2->left;
            }
            pos++;
        }
        printf(".\n");
        printf(">> Object 1 is different from object 2:\n it is");        
    for (; pos < descr1Size && pos < descr2Size; pos++)
    {   
        if (description1.getDataOnPos(pos))
        {
            printf(" %s,", node1->data);
            node1 = node1->left;
        }
        else
        {
            printf(" not %s,", node2->data);
            node2 = node2->right;
        }
    }
    printf(".\n");
    
    
    description1.stackDtor();
    description2.stackDtor();

    return 0;
}
