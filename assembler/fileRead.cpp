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

int spacesToZeroes(char *code, const int fileSize, int *wordCount)
{
    if (code == NULL || fileSize == 0 || wordCount == NULL)
        return 1;
    
    int i = 0;

    while (code[i] != '\0')
    {
        if (code[i] == ';')
            while (code[i] != '\n' && code[i] != '\r' && code[i] != '\0')
                i++;

        if (code[i] == '[')
            do
            {
                assert(i < fileSize);
                i++;
            } while (code[i] != ']');
            
        
        if ((!isspace(code[i + 1]) || !isspace(code[i - 1])) && isspace(code[i]) || code[i] == ',')
        {
            code[i] = '\0';            
            if (!isspace(code[i + 1]))
                (*wordCount)++;
        }
        i++;
    }
    
    if (code[i - 1] != '\0')
        (*wordCount)++;
    

    return 0;
}
            
int breakToWords(char *code, char ***wordsPtrs, int *wordCount)
{
    char **tempArr = (char **)calloc(*wordCount, sizeof(char *));
    if (tempArr == NULL)
    {
        printf("Memory allocation error.");      
        return 1;  
    }
        
    tempArr[0] = code;
    for (int i = 1; i < *wordCount; i++)
    {
        tempArr[i] = strchr(tempArr[i - 1], '\0') + 1;
        while (*tempArr[i] == '\0' || *tempArr[i] == '\r')
            tempArr[i] = strchr(tempArr[i], '\0') + 1;
    }

    *wordsPtrs = tempArr;
    /*if (wordCount != NULL)
        *wordCount = strNum;*/

    return 0;
}

int fileRead(const char *fileName, char ***strArrPtr, int *wordCount)
{
    assert(fileName != NULL);
    assert(strArrPtr != NULL);
    
    char *code = NULL;
    int fileSize = 0;
    
    readToBuff(fileName, &code, &fileSize);
    spacesToZeroes(code, fileSize, wordCount);
    breakToWords(code, strArrPtr, wordCount);

    return 0;
}