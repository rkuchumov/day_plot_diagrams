#include "utils.h"
#include "params.h"

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <stdint.h>
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

float read_flt(FILE *fp) 
{
    union u4 flt_u;
    uint8_t d[4];

    fread(d, 1, 4, fp);

    flt_u.data[0] = d[3];
    flt_u.data[1] = d[2];
    flt_u.data[2] = d[1];
    flt_u.data[3] = d[0];

    /* flt_u.data[0] = d[0]; */
    /* flt_u.data[1] = d[1]; */
    /* flt_u.data[2] = d[2]; */
    /* flt_u.data[3] = d[3]; */

    /* debug("%#02x %#02x %#02x %#02x --> %f",  */
    /*         flt_u.data[0],  */
    /*         flt_u.data[1],  */
    /*         flt_u.data[2],  */
    /*         flt_u.data[3],  */
    /*         flt_u.f); */

    /* sleep(1); */

    return flt_u.i;
}

float read_int(FILE *fp) 
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

    /* debug("%#02x %#02x %#02x %#02x --> %f",  */
    /*         int_u.data[0],  */
    /*         int_u.data[1],  */
    /*         int_u.data[2],  */
    /*         int_u.data[3],  */
    /*         int_u.f); */

    return int_u.i;
}

void set_tz(char *env_tz)
{
    assert(env_tz != NULL);
#if HAVE_SETENV 
	if (setenv("TZ", env_tz, 1) < 0)
        fatal_errno("setenv");
#else 
    int len = strlen("TZ") + 1 + strlen(env_tz) + 1; 
    char *str = malloc(len); 
    if (str == NULL)
        fatal_errno("malloc");

    sprintf(str, "TZ=%s", env_tz); 

    if (putenv(str) != 0)
        fatal_errno("putenv");
    free(str);
#endif

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

#if !(HAVE_GETLINE)
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (long)(size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif
