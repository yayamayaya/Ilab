#include "../main.h"

extern FILE *akLog;
extern const char *format;

int takeAString(char **strPtr)
{
    int scanned = 0;
    char *string = (char *)calloc(MAX_STRING_LENGTH, sizeof(char));

    do 
    {
        while (getchar() != '\n')
            continue;
        scanned = scanf("%[^\n]", string);
    } while (!scanned);

    if (!string)
    {
        LOG(format, "NULL_STR", NULL_STR, "[error]");
        return NULL_STR;
    }
    *strPtr = string;
    return 0;
}

int akinatorAsk(const char *format, const char *string)
{
    assert(format);

    printf(format , string);
    printf(" [y/n]\n");    

    int scanned = 0;
    char answer = 0;

    do 
    {
        scanned = scanf(" %c", &answer);
    } while (!scanned && (answer != 'y' || answer != 'n'));

    if (answer == 'y')
        return YES;
    else if (answer == 'n')
        return NO;
    
    LOG(format, "YES_OR_NO_ERR", YES_OR_NO_ERR, "[error]");
    return YES_OR_NO_ERR;        
}

int addDescription(node_t *node)
{
    assert(node);

    int scanned = 0;

    if (node->left && node->right)
    {
        LOG(format, "NEW_DESCRIPTION_ERR", NEW_DESCRIPTION_ERR, "[error]");
        return NEW_DESCRIPTION_ERR;
    }

    node_t *rightNodeAnswer = (node_t *)calloc(1, sizeof(node_t));
    node_t *leftNodeAnswer = (node_t *)calloc(1, sizeof(node_t));
    if (!rightNodeAnswer || !leftNodeAnswer)
    {
        LOG(format, "MEM_ALC_ERR", 0, "[error]");
        return MEM_ALC_ERR;
    }
    node->right = rightNodeAnswer;
    node->left = leftNodeAnswer;

    rightNodeAnswer->data = node->data;

    printf(">>  Then who is it?\n");
    if (takeAString(&leftNodeAnswer->data))
        return NULL_STR;

    printf(">>  And how is %s different from %s?\n", leftNodeAnswer->data, rightNodeAnswer->data);
    if (takeAString(&node->data))
        return NULL_STR;

    printf(">>  Good! I will remember that!\n");

    return 0;
}

void addAnswer(node_t *node)
{
    assert(node);

    printf(">>  I don't know such character.. Who is it?\n");
    takeAString(&node->data);
    
    return;
}

int Akinator(node_t *treeNode)
{
    assert(treeNode);

    while (1)
    {
        if (!treeNode->left && !treeNode->right)
        {
            
            if (akinatorAsk(">>  Is it %s?", treeNode->data))
            {
                printf(">>  I knew it!\n");
                LOG(format, "ANSWER_FOUND", ANSWER_FOUND, "");
                return ANSWER_FOUND;
            }
            else
                if(addDescription(treeNode))
                    return NEW_DESCRIPTION_ERR;
                else
                {
                    LOG(format, "DESCRIPTION_ADDED", DESCRIPTION_ADDED, "");
                    return DESCRIPTION_ADDED;
                }
        } 

        if (akinatorAsk(">>  %s?\n", treeNode->data))
            if (treeNode->left)
            {
                treeNode = treeNode->left;
                continue;
            }
            else
            {
                addAnswer(treeNode->left);
                LOG(format, "ANSWER_ADDED", ANSWER_ADDED, "");
                return ANSWER_ADDED;
            }
        else
            if (treeNode->right)
            {
                treeNode = treeNode->right;
                continue;
            }
            else
            {
                addAnswer(treeNode->right);                
                LOG(format, "ANSWER_ADDED", ANSWER_ADDED, "");
                return ANSWER_ADDED;
            }
    }
}