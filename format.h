#ifndef FORMAT_H
#define FORMAT_H

#include <time.h>
#include <float.h>

// typedef double samp_t;
// #define SAMP_MAX DBL_MAX
// #define SAMP_MIN -DBL_MAX

typedef float samp_t;
#define SAMP_MAX FLT_MAX
#define SAMP_MIN -FLT_MAX

struct data_t {
    samp_t *d;
    int samp_cnt;
    double samp_rate;
    time_t time;
};

struct data_t **read_data();

#endif /* end of include guard: FORMAT_H */

