#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "fileReader.h"
#include <ctype.h>

#define DEBUG
#define FILE_SIZE "\n>>Size of file is: %d\n", size
#define STRING_NUMBER ">>Number of strings is: %d\n\n", strs

#ifdef DEBUG
    #define PRINT(arg) printf(arg);
#else
    #define PRINT(arg) 
#endif

void sizeOfFile(FILE * fileName, int *fileSize) 
{ 
    fseek(fileName, 0, SEEK_END);
    *fileSize = ftell(fileName);
}

void numberOfStrings(char* buffName, int* stringNumber)        //Подумать над пропуском строк
{
    int i = 0;
    int count = 0;      

    while (buffName[i] != '\0')
    {
        if (buffName[i] == '\r')
        {
            buffName[i] = '\0';
            count++;
            i += 2;
        }
        else if (buffName[i] == '\n')
        {
            buffName[i] = '\0';
            count++;
        }
        i++;
    }

    if(buffName[i - 1] != '\0' && buffName[i - 1] != '\n')
        count++;

    *stringNumber = count;
}

int readToBuff(char *fileName, char **buffPtr, int *fileSize)
{
    FILE * file = fopen(fileName, "rb");              //Открываем файл, проверка на существование файла и его открытие
    if(file == NULL)
    {
        printf(">>>Can't open the file.\n");
        return 1;
    }

    int size = 0;                                           //Размер файла в символах

    sizeOfFile(file, &size);

    char *Buff = (char *)calloc(size + 1, sizeof(char));       //Создаем массив, куда запишем весь файл
    if (Buff == NULL)
    {
        return 1;
    }

    rewind(file);                                        //Возвращаемся в начало файла
    if (fread(Buff, sizeof(char), size, file) < size)       //Условие на ошибки
    {
        printf(">>>Text reading error.\n");
        return 1;
    }
    fclose(file);

    *buffPtr = Buff;
    if (fileSize != NULL)
        *fileSize = size;

    return 0;
}

int buffToStrArr(char *buffName, char ***strArrPtr, int *stringNumber)
{
    int strNum = 0;

    numberOfStrings(buffName, &strNum);
    if (strNum == 0)
        return 1;

    char **tempArr = (char **)calloc(strNum, sizeof(char *));
    if (tempArr == NULL)
    {
        printf("Memory allocation error.");      
        return 1;  
    }
        
    tempArr[0] = buffName;
    for (int i = 1; i < strNum; i++)
    {
        tempArr[i] = strchr(tempArr[i - 1], '\0') + 1;
        if (*tempArr[i] == '\n')
            tempArr[i] += 1;
    }

    *strArrPtr = tempArr;
    if (stringNumber != NULL)
        *stringNumber = strNum;

    return 0;
}

int fileRead(char *fileName, char** buffPtr, char ***strArrPtr, int *fileSize, int *stringNumber, int option)
{
    assert(fileName != NULL);
    assert(buffPtr != NULL || strArrPtr != NULL);
    assert(1 <= option && option <= 3);
    
    char *Buff = NULL;
    
    readToBuff(fileName, &Buff, fileSize);

    if(option != STRS_ONLY)
        *buffPtr = Buff;
    if(option != BUFF_ONLY)
        buffToStrArr(Buff, strArrPtr, stringNumber);
}

void spacesToZeroes(char **strArrPtr, const int stringNumber)
{
    char *spacePtr = NULL;
    for (int i = 0; i < stringNumber; i++)
    {
        spacePtr = strchr(strArrPtr[i], ' ');
        while(spacePtr != NULL)
        {
            *spacePtr = '\0';
            spacePtr = strchr(strArrPtr[i], ' ');
        }
    }
}