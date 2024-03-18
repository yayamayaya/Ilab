#pragma once

void sizeOfFile(FILE * fileName, int *fileSize);
int readToBuff(const char *fileName, char **pcode, int *pfileSize);
int specialChToZero(char *code, const int fileSize, int *wordCount);
int Tokenization(char *code, char ***tokenArr, int *wordCount);
int fileRead(const char *fileName, char** bufferPtr, char ***strArrPtr, int *wordCount);