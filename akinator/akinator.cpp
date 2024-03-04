#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <../common/log.h>
#include "../BinaryTree/BinaryTree.h"

FILE *akLog = NULL;

int main(const int argc, const char const *argv[])
{
#ifdef DEBUG
    akLog = openLogFile(3, argv, "akinatorLog.log", "LOG for Akinator");
#endif
    //Происходит чтение файла

    //Происходит акинация



    return 0;
}

int Akinator(Node_t *treeNode)
{
    assert(treeNode);
    /*if (!treeNode->left && !treeNode->right)
    {
        fprintf("Is it %s?\n", treeNode->data);
        if (/* condition )
        {
            /* code 
        }
        
    }*/
    
    
    while (1)
    {
        fprintf(stderr, "%s?\n", treeNode->data);

        int scanNum = 0;
        char answer = 0;
        do
        {
            scanNum = scanf("%c", &answer);
        } while (!scanNum);
        
        if (answer == 'y')
            if (treeNode->left)
            {
                treeNode = treeNode->left;
                continue;
            }
            else
                //answerNotFound
        else if (answer == 'n')
            if (treeNode->right)
            {
                treeNode = treeNode->right;
                continue;
            }
            else
                //answerNotFound
        
    }
    

}



