#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "fileReader.h"
#include "Bsort.h"
#include "Swap.h"
#include <ctype.h>

typedef struct string {
    char *strPointer;
    int strLeng;
} string;

int strCmp(void *string1, void *string2);
void reverseBsort(char **arrPoint, int size);
int reverseStrCmp(void *s1, void *s2);
void StrPrint(char **arrPoint, int strNumber);
void stringPrint(string *strings, int strNumber);

int main(int argc, char **argv) 
{
    char *Buff = NULL;
    int size = 0;
    fileRead(argv[1], &Buff, NULL, &size, NULL, BUFF_ONLY);  //Читаем файл в буффер

    int strs = 0;
    numberOfStrings(Buff, &strs);
    printf("\n>>Number of strings is: %d\n", strs);

    string *stroki = (string *)calloc(strs, sizeof(string));          //Создаём массив указателей на начала строк, размер массива - количество строк в тексте

    stroki[0].strPointer = Buff;
    for (int i = 1; i < strs; i++)
    {
        stroki[i].strPointer = strchr(stroki[i - 1].strPointer, '\0') + 1; 
        stroki[i - 1].strLeng = stroki[i].strPointer - stroki[i - 1].strPointer;    //Длина строки без учета \n, если он есть(в байтах)
        if(*stroki[i].strPointer == '\n')
            stroki[i].strPointer += 1;
    }
    stroki[strs - 1].strLeng = &Buff[size + 1] - stroki[strs - 1].strPointer;

    stringPrint(stroki, strs);

    printf("\nSorting in alphabet order:\n\n");
    Bsort(stroki, strs, sizeof(string), strCmp);
    stringPrint(stroki, strs);

    printf("\nSorting in alphabet order from the end:\n\n");
    Bsort(stroki, strs, sizeof(string), reverseStrCmp);
    stringPrint(stroki, strs);
    
    printf("\nPrinting original text: \n\n");
    for(int i = 0; i < size; i++)
    {
        if(i != 0 && Buff[i] != '\n' && Buff[i - 1] == '\0')
            printf("\n");        
        printf("%c", Buff[i]);
    } 

    free(stroki);
    free(Buff);
}

int strCmp(void *point1, void *point2)                              //Сравнение строк
{
    int pos = 0;
    string *p1 = (string *)point1;
    string *p2 = (string *)point2;
    short int ch1 = 0;
    short int ch2 = 0;

    for (int pos1 = 0, pos2 = 0; *((p1-> strPointer) + pos1) != '\0' && *((p2-> strPointer) + pos2) != '\0';)
    {
        if (*((p1-> strPointer) + pos1) & 0x80)
        {
            ch1 = *(uint16_t *)((p1-> strPointer) + pos1);             //Делаем 2 бегунка, прибавляем два байта, если нужно, 
            pos1 += sizeof(uint16_t);
        }
        else
        {
            ch1 = *((p1-> strPointer) + pos1);          
            pos1++;  
        }
        if (*((p2-> strPointer) + pos2) & 0x80)
        {
            ch2 = *(uint16_t *)((p2-> strPointer) + pos2);             //Делаем 2 бегунка, прибавляем два байта, если нужно, 
            pos2 += sizeof(uint16_t);
        }
        else
        {
            ch2 = *((p2-> strPointer) + pos2);          
            pos2++;  
        }
        
        if (ch1 == ch2)
            continue;

        return ch1 - ch2;
    }
}

int reverseStrCmp(void *point1, void *point2)                       //Сравнение строк с конца
{
    string *p1 = (string *)point1;
    string *p2 = (string *)point2;

    int pos1 = p1-> strLeng - 2;
    int pos2 = p2-> strLeng - 2;

    unsigned short int ch1 = 0;
    unsigned short int ch2 = 0;

    /*while(isalpha(*((p1-> strPointer) + pos1)) == 0 && *((p1-> strPointer) + pos1) != '\0')          //Игнорируем знаки препинания
        --pos1;
    while(isalpha(*((p2-> strPointer) + pos2)) == 0 && *((p1-> strPointer) + pos1) != '\0')       
        --pos2;*/
    
    for (; *((p1-> strPointer) + pos1) != '\0' && *((p2-> strPointer) + pos2) != '\0';)
    {
        if (*((p1-> strPointer) + pos1) & 0x80)
        {
            ch1 = ((*(uint8_t *)((p1-> strPointer) + pos1 - 1) ^ 0xC0) << 8) | (*(uint8_t *)((p1-> strPointer) + pos1) ^ 0x80);             //Делаем 2 бегунка, прибавляем два байта, если нужно,
            pos1 -= 2;
        }
        else
        {
            ch1 = *((p1-> strPointer) + pos1);          
            pos1--;  
        }
        if (*((p2-> strPointer) + pos2) & 0x80)
        {
            ch2 = ((*(uint8_t *)((p2-> strPointer) + pos2 - 1) ^ 0xC0) << 8) | (*(uint8_t *)((p2-> strPointer) + pos2) ^ 0x80);              //Делаем 2 бегунка, прибавляем два байта, если нужно, 
            pos2 -= 2;
        }
        else
        {
            ch2 = *((p2-> strPointer) + pos2);          
            pos2--;
        }
        
        if (ch1 == ch2)
            continue;

        return ch1 - ch2;
    }
}

void stringPrint(string* strings, int strNumber)
{
    for (int i = 0; i < strNumber; i++)
        puts((strings + i)-> strPointer );
}

//Сравнение uint_16 символов между собой