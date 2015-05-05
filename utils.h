#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define SEC_PER_DAY 24 * 60 * 60

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define DATE_LEN 60

void printlog(unsigned vlevel, const char *file, int line, 
        const char *fmt, ...);

char *m_mktemp();

/** Reads 4 byte from specified file and converts it to float
 *
 * @param fp file pointer to read header from
 * @return float value
 */
float read_flt(FILE *fp);

int read_int(FILE *fp);

void set_tz(char *env_tz);

time_t day_start(time_t t);

ssize_t m_getline(char **lineptr, size_t *n, FILE *stream);

bool is_dir(const char *path);

struct tm *m_localtime(const time_t *timep);

#define FATAL 0
#define DEBUG 1
/** buffer size for log messages */
#define LOG_MSG_SIZE 400

#define fatal(...)       printlog(FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define fatal_errno(str) printlog(FATAL, __FILE__, __LINE__, "%s() error: %s", str, strerror(errno))
#define debug(...)       printlog(DEBUG, __FILE__, __LINE__, __VA_ARGS__)

#endif /* end of include guard: UTILS_H */

