#pragma once
#include <stdio.h>

void fileLog(FILE *logFileName, const char *format, ...) __attribute__ ((format(printf, 2, 3)));
FILE *openLogFile(const int argc, const char **argv, const char *StandardFileName, const char *format, ...);
