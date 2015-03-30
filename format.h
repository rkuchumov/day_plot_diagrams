#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <time.h>

struct data_t {
    float *d;
    int samp_cnt;
    double samp_rate;
    time_t time;
};

struct data_t **read_data(FILE *wfdisc_file);

#endif /* end of include guard: FORMAT_H */

