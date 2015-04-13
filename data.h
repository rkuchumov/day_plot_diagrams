#ifndef DATA_H
#define DATA_H

#include "format.h"


void butterworth_bandpass(struct data_t **data, int n);

void slope(struct data_t **data, int n);

void scale(struct data_t **data, int n);

#endif /* end of include guard: DATA_H */

