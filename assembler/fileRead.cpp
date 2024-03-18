#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


void sizeOfFile(FILE * fileName, int *fileSize) 
{ 
    fseek(fileName, 0, SEEK_END);
    *fileSize = ftell(fileName);
}

int readToBuff(const char *fileName, char **pcode, int *pfileSize)
{
    FILE * file = fopen(fileName, "rb");              //Открываем файл, проверка на существование файла и его открытие
    if(file == NULL)
    {
        printf(">>>Can't open the file.\n");
        return 1;
    }

    int size = 0;                                           //Размер файла в символах
    sizeOfFile(file, &size);
    if(size == 0)
        printf(">>> File is empty\n");

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

    *pcode = Buff;
    *pfileSize = size;

    return 0;
}

int specialChToZero(char *code, const int fileSize, int *wordCount)
{
    assert(code);
    assert(wordCount);
    int character = 0;

    for (; character < fileSize; character++)
    {
        if (code[character] == ';')
            while ((!isspace(code[character]) || code[character] == ' ') && code[character] != '\0' )
                character++;
        
        if (isspace(code[character]) || (!isalnum(code[character]) && code[character] != '-' && code[character] != ':'))
        {
            code[character] = '\0';
            if (isalnum(code[character + 1]) || code[character + 1] == '-')
                (*wordCount)++;
        }
    }
    if (code[0] != '\0')
        (*wordCount)++;

    return 0;
}

int Tokenization(char *code, char ***tokenArr, int *wordCount)
{
    assert(code);

    char **tempArr = (char **)calloc(*wordCount, sizeof(char *));
    if (tempArr == NULL)
    {
        printf("Memory allocation error.");      
        return 1;  
    }
    for (int i = 0, counter = 0; counter < *wordCount; i++)
        if (isalnum(code[i]) || code[i] == '-')
        {
            tempArr[counter] = code + i;
            i = strchr(code + i, '\0') - code;
            ++counter;
        }

    *tokenArr = tempArr;

    return 0;
}

int fileRead(const char *fileName, char **bufferPtr, char ***strArrPtr, int *wordCount)
{
    assert(fileName != NULL);
    assert(strArrPtr != NULL);
    
    char *code = NULL;
    int fileSize = 0;
    
    readToBuff(fileName, &code, &fileSize);
    specialChToZero(code, fileSize, wordCount);
    Tokenization(code, strArrPtr, wordCount);

    *bufferPtr = code;

    return 0;
}