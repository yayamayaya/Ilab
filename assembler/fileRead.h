#pragma once

void sizeOfFile(FILE * fileName, int *fileSize);
int readToBuff(const char *fileName, char **pcode, int *pfileSize);
int spacesToZeroes(char *code, const int fileSize, int *wordCount);
int breakToWords(char *code, char ***wordsPtrs, int *wordCount);
int fileRead(const char *fileName, char ***strArrPtr, int *wordCount);