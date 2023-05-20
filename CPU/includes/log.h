#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdio.h>

#define NDEBUG

#ifndef NDEBUG
    #define ASSERT(condition)                                                                           \
    if(!(condition))                                                                                    \
    {                                                                                                   \
    printf("----------------\nError in %s\nPlace: %s:%d\nFUNC: %s\n----------------\n",                 \
             #condition, __FILE__, __LINE__, __PRETTY_FUNCTION__);                                      \
    }
#else
    #define ASSERT(condition) ;
#endif

#ifndef LOG_NAME
    #define LOG_NAME "log.txt"
#endif

#ifndef NDEBUG

#define logprint(str...)                                                 \
    fprintf(LogFile, str);                                               \
    fflush(LogFile)  

#else

#define logprint(str...)                                                     

#endif

extern FILE* LogFile;

FILE* openLog();
void CloseLog();

#endif // LOG_H_INCLUDED
