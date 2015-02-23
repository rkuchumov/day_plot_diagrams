#include "utils.h"
#include "params.h"

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

void printlog(unsigned vlevel, const char *file, int line, 
        const char *fmt, ...)
{
    assert(file != NULL);
    assert(fmt != NULL);

    if (!cfg.debug_out && vlevel != FATAL)
        return;

    va_list args;
    va_start(args, fmt);

    static char logbuf[LOG_MSG_SIZE];
    char *end = logbuf;

    if (vlevel == FATAL)
        end += sprintf(end, "[FATAL]");
    else if (vlevel == DEBUG)
        end += sprintf(end, "[DEBUG]");

    end += sprintf(end, "[%s:%d] ", file, line);

    end += vsprintf(end, fmt, args);

    end += sprintf(end, "\n");

    fprintf(stderr, "%s", logbuf);

    va_end(args);

    if (vlevel == FATAL)
        exit(EXIT_FAILURE);
}
