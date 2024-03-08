#pragma once
#include <stdio.h>

void fileLog(FILE *logFileName, const char *format, ...) __attribute__ ((format(printf, 2, 3)));
FILE *openLogFile(const int argNum, const char **argv, const char *StandardFileName, const char *format, ...);

enum LOG_ERRORS
{
    LOG_FILE_OPEN_ERR   = 1010,

};
