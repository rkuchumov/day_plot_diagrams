#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void printlog(unsigned vlevel, const char *file, int line, 
        const char *fmt, ...);

char *m_mktemp();

#define FATAL 0
#define DEBUG 1
/** buffer size for log messages */
#define LOG_MSG_SIZE 150

#define fatal(...)       printlog(FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define fatal_errno(str) printlog(FATAL, __FILE__, __LINE__, "%s() error: %s", str, strerror(errno))
#define debug(...)       printlog(DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#endif /* end of include guard: UTILS_H */

