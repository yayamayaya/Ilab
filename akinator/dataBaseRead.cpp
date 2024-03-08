#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "akinator.h"
#include "../BinaryTree/BinaryTree.h"

/*int main(int argc, char const *argv[])
{
    char *buff = NULL;
    int fileSize = 0;

    readToBuff("../akinator/dataBase.txt", &buff, &fileSize);
    if (buff == NULL)
    {
        fprintf(stderr, "FILEREAD ERROR");
        return 1;
    }

    fprintf(stderr, "%s", buff);

    int pos = 0;
    node_t *tree = readDataBase(buff, &pos);

    treePrint(stderr, tree);
    treeKill_string(tree);
    free(buff);

    fprintf(stderr, "win");


    return 0;
}*/

//{"some text"{}{}}
//"\"%[^\"]\"%n"

node_t *readDataBase(char *buff, int *pos)
{
    assert(buff[*pos] == '{');

    //int data = 0;
    int scanned = 0;

    //fprintf(stderr, "Found '{'\n");
    (*pos)++;

    //fprintf(stderr, "pos shifted, now - %d, the rest: [ %s ]\n", *pos, buff + *pos);
    if (buff[*pos] == '}')
    {
        //fprintf(stderr, "found '}', return NULL\n");
        (*pos)++;
        return NULL;
    }
    
    char *string = (char *)calloc(MAX_STRING_LENGTH, sizeof(char));
    if (string == NULL)
    {
        fprintf(stderr, "STR_MEM_ALC_ERR, %p\n", string);
        return NULL;
    }
    
    sscanf(buff + *pos, "\"%[^\"]\"%n", string, &scanned);
    if (scanned == 0)
    {
        free(string);
        return NULL;        
    }

    //fprintf(stderr, "number scanned: %s, symbols scanned: %d\n", string, scanned);
    
    *pos += scanned;
    //fprintf(stderr, "position shifted: %d, The rest: [ %s ]\n", *pos, buff + *pos);

    node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
    if (!newNode)
    {
        fprintf(stderr, "MEM_ALC_ERR\n");
        return NULL;
    }
    //fprintf(stderr, "Node allocated: %p\n", newNode);
    newNode->data = string;

    if (buff[*pos] == '{')
        newNode->left = readDataBase(buff, pos);
    if (buff[*pos] == '{')
        newNode->right = readDataBase(buff, pos);

    if (buff[*pos] == '}')
    {
        (*pos)++;
        //fprintf(stderr, "position shifted, end of node: %d, The rest: [ %s ]\n", *pos, buff + *pos);
    }
        
    else
    {
        fprintf(stderr, "syntax error: %d, The rest: [ %s ]\n", *pos, buff + *pos);
        return NULL;
    }

    return newNode;
}
    

    


