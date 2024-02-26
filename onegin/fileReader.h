#pragma once

enum options
{
    BUFF_ONLY = 1,
    STRS_ONLY = 2, //Хотим мы записать буфер, строки или и то, и другое
    BOTH = 3
};


void sizeOfFile(FILE * fileName, int *fileSize);
void numberOfStrings(char* buffName, int* stringNumber);
int readToBuff(char *fileName, char **buffPtr, int *fileSize);
int buffToStrArr(char *buffName, char ***strArrPtr, int *stringNumber);
int fileRead(char *fileName, char** text ,char ***strPointer, int *sizeOfFile, int *numberOfStrings, int option);
void spacesToZeroes(char **strArrPtr, const int fileSize);