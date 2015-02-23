#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>

/** Parses file header into cfg structure. Sets current file position
 * to the beginning of relevant data
 *
 * @param fp file pointer to read header from
 * @return -1 on error, 1 otherwise
 */
int parse_header(FILE *fp);

void parse_header_opt(char *name, char *value);

/** Reads 4 byte from specified file and converts it to float
 *
 * @param fp file pointer to read header from
 * @return float value
 */
float read_flt(FILE *fp);

#endif /* end of include guard: FORMAT_H */

