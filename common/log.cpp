#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

void fileLog(FILE *logFileName, const char *format, ...)
{
    assert(logFileName);

    va_list arg = {};
    va_start(arg, format);
    vfprintf(logFileName, format, arg);
    va_end(arg);

    fflush(logFileName);
}


//Название логфайла передаётся последним в аргв
FILE *openLogFile(const int argc, const char **argv, const char *StandardFileName, const char *format, ...)
{
    assert(argv);
    assert(StandardFileName);

    FILE *logFile = NULL;

    if (argv[argc - 1] != NULL)
        logFile = fopen(argv[argc - 1], "w");
    else
        logFile = fopen(StandardFileName, "w");
    
    if (logFile == NULL)
    {
        fprintf(stderr, "Can't open logFile.");
        return NULL;
    }

    va_list args = {};
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    return logFile;
}