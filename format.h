#ifndef FORMAT_H
#define FORMAT_H

#include <time.h>

struct data_t {
    float *d;
    int samp_cnt;
    double samp_rate;
    time_t time;
};

struct data_t **read_data();

#endif /* end of include guard: FORMAT_H */

