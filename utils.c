#include "utils.h"
#include "params.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <sys/stat.h>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

union u4 {
    float f;
    int32_t i;
    uint32_t u;

    uint8_t data[4];
};

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
#ifndef WIN32
    static char const *template = "/tmp/geodiagrams_XXXXXX";
#else
    static char const *template = "geodiagrams_aXXXXXX";
#endif

    int len = strlen(template);

    char *filename = (char*) malloc(len + 1);
    if (filename == NULL)
        fatal_errno("malloc");

    strcpy(filename, template);

#ifndef WIN32
    int fd;
    if ((fd = mkstemp(filename)) == -1)
        fatal_errno("mkstemp");

    close(fd);
#else
again:
    if (_mktemp(filename) == NULL) {
        if (errno != EEXIST)
            fatal_errno("_mktemp");

        strcpy(filename, template);
        filename[strlen(template) - 7]++;
        goto again;
    }
#endif

    return filename;
}

int read_int(FILE *fp) 
{
    union u4 int_u;
    uint8_t d[4];

    fread(d, 1, 4, fp);

    int_u.data[0] = d[3];
    int_u.data[1] = d[2];
    int_u.data[2] = d[1];
    int_u.data[3] = d[0];

    /* int_u.data[0] = d[0]; */
    /* int_u.data[1] = d[1]; */
    /* int_u.data[2] = d[2]; */
    /* int_u.data[3] = d[3]; */

    /* debug("%#02x %#02x %#02x %#02x --> %d",  */
    /*         int_u.data[0],  */
    /*         int_u.data[1],  */
    /*         int_u.data[2],  */
    /*         int_u.data[3],  */
    /*         int_u.i); */

    return int_u.i;
}

void set_tz(char *env_tz)
{
    assert(env_tz != NULL);

    int len = strlen("TZ") + 1 + strlen(env_tz) + 1; 
    char *str = malloc(len); 
    if (str == NULL)
        fatal_errno("malloc");

    sprintf(str, "TZ=%s", env_tz); 

    if (putenv(str) != 0)
        fatal_errno("putenv");

    /* nope, don't free it */
    /* free(str); */

	tzset();
}

time_t day_start(time_t t)
{
    struct tm *ptm = localtime(&t);
    ptm->tm_hour = 0;
    ptm->tm_min = 0;
    ptm->tm_sec = 0;
    return mktime(ptm);
}

ssize_t m_getline(char **lineptr, size_t *n, FILE *stream) {
    assert(n != NULL);
    assert(stream != NULL);
    assert(lineptr != NULL);

    char *p = NULL;
    char *bufptr = *lineptr;
    size_t size = *n;

    int c = fgetc(stream);
    if (c == EOF)
        return -1;

    if (bufptr == NULL) {
        if ((bufptr = malloc(128)) == NULL)
            fatal_errno("malloc");
        size = 128;
    }

    p = bufptr;
    while (c != EOF) {
        if ((p - bufptr) > (long)(size - 1)) {
            size += 128;
            if ((bufptr = realloc(bufptr, size)) == NULL)
                fatal_errno("realloc");
            p = bufptr + size - 128;
        }
        *p++ = c;
        if (c == '\n')
            break;
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}

bool is_dir(const char *path)
{
    assert(path != NULL);

    struct stat s;
    if (stat(path, &s) < 0)
        fatal_errno("stat");

    if (s.st_mode & S_IFDIR)
        return true;

    return false;
}
