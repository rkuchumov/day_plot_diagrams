#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>

FILE *next_file(FILE *wfdisc_fp);

/** Reads 4 byte from specified file and converts it to float
 *
 * @param fp file pointer to read header from
 * @return float value
 */
float read_flt(FILE *fp);

#endif /* end of include guard: FORMAT_H */

