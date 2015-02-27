#include "utils.h"
#include "params.h"

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#ifndef WIN32
#include <unistd.h>
#endif

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

char *m_mktemp()
{
    static char const *template = "/tmp/geodiagrams_XXXXXX";
    int len = strlen(template);

    char *filename = (char*) malloc(len + 1);
    if (filename == NULL)
        fatal_errno("malloc");

    strcpy(filename, template);

    int fd;
    if ((fd = mkstemp(filename)) == -1)
        fatal_errno("mktemp");

    close(fd);

    return filename;
}
