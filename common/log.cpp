#include "log.h"

void fileLog(FILE *logFileName, const char *format, ...)
{
#ifdef DEBUG
    assert(logFileName);

    va_list arg = {};
    va_start(arg, format);
    vfprintf(logFileName, format, arg);
    va_end(arg);

    fflush(logFileName);
#endif
}


//Название логфайла передаётся последним в аргв
FILE *openLogFile(const int argNum, const char **argv, const char *StandardFileName, const char *format, ...)
{
    FILE *logFile = NULL;    
#ifdef DEBUG
    assert(argv);
    assert(StandardFileName);
    setbuf(stdout, NULL);



    if (argv[argNum - 1] != NULL)
        logFile = fopen(argv[argNum - 1], "w");
    else
        logFile = fopen(StandardFileName, "w");
    
    if (logFile == NULL)
    {
        fprintf(stdout, "Can't open logFile.");
        return NULL;
    }
    setbuf(logFile, NULL);

    va_list args = {};
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    fflush(logFile);
#endif
    return logFile;
}