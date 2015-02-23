#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <stdint.h>

#include "utils.h"
#include "format.h"
#include "params.h"

union u4 {
    float f;
    int32_t i;
    uint32_t u;

    uint8_t data[4];
};

int parse_header(FILE *fp)
{
    assert(fp != NULL);

    char *line = NULL;
    size_t len = 0;

    int key_len;
    int value_len;
    char *value;

    char *value_cpy = NULL;

    unsigned bytes_read = 0;

    debug("Parsing header");

    while (fgetc(fp) != '\n')
        bytes_read++;

    ssize_t rc = 0;
    while ((rc = getline(&line, &len, fp)) > 0) {
        bytes_read += rc;

        for (int i = 0; i < rc; i++) {
            if ('=' == line[i]) {
                /* skip spaces before '=' */
                int j = i - 1;
                while (j >= 0 && strchr(" \t", line[j])) j--;

                key_len = j + 1; 
                line[key_len] = '\0';

                /* skip spaces after '=' */
                j = i + 1;
                while (j < rc && strchr(" \t", line[j])) j++;

                value = &line[j]; 

            } else if ('\n' == line[i]) {
                /* skip spaces before '\n' */
                int j = i - 1;
                while (j >= 0 && strchr(" \t", line[j])) j--;

                line[j + 1] = '\0';
                value_len = j + 1 - (value - line); 
                break;
            } 
        }

        if ((value_cpy = (char *) malloc(value_len)) == NULL)
            fatal_errno("malloc");

        if (*value == '"' && value[value_len - 1] == '"') {
            memcpy(value_cpy, value + 1, value_len - 1);
            value_cpy[value_len - 2] = '\0';
        } else {
            memcpy(value_cpy, value, value_len);
            value_cpy[value_len - 1] = '\0';
        }

        parse_header_opt(line, value_cpy);

        int c = fgetc(fp);
        if (fseek(fp, -1, SEEK_CUR) < 0)
            fatal_errno("fseek");
        if (c == 0)
            break;
    }

    if (line != NULL)
        free(line);

    if (bytes_read > cfg.data_offset)
        fatal("Header size is greater than data offset");

    if (fseek(fp, cfg.data_offset - bytes_read - 1, SEEK_CUR) < 0)
        fatal_errno("fseek");

    return 1;
}

void parse_header_opt(char *name, char *value)
{
    /* debug("%s => %s ", name, value); */

    char *endptr;
    int val_num = strtol(value, &endptr, 10);

    if (strcmp(name, "Data offset") == 0) {
        cfg.data_offset = val_num;
        if (*endptr != '\0')
            fatal("%s is expected to be a number", name);
    } else if (strcmp(name, "Data Type") == 0) {
        if (strcmp(value, "FLOAT") == 0)
            cfg.data_type = FLOAT;
        /* XXX: is there other formats? */
    }
}

float read_flt(FILE *fp) 
{
    union u4 flt_u;

    // XXX: handle endians
    fread(flt_u.data, 1, 4, fp);

    /* debug("%#02x %#02x %#02x %#02x --> %f",  */
    /*         flt_u.data[0],  */
    /*         flt_u.data[1],  */
    /*         flt_u.data[2],  */
    /*         flt_u.data[3],  */
    /*         flt_u.f); */

    return flt_u.f;
}
