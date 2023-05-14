#include <stdio.h>
#include <stdlib.h>

#include "../includes/log.h"

FILE* LogFile = openLog();


FILE* openLog() {
    FILE *Log = fopen(LOG_NAME, "w");
    fprintf(Log, "--------------------------------------------------\n");
    fprintf(Log, "log file is opened\n\n");
    atexit(CloseLog);

    return Log;
}

void CloseLog() {
    fprintf(LogFile, "\n\nlog file is closed\n");
    fprintf(LogFile, "--------------------------------------------------\n");
    fclose(LogFile);
}