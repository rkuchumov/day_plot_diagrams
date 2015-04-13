#include "utils.h"
#include "params.h"
#include "data.h"
#include <float.h>
#include <assert.h>
#include <math.h>

#include <stdlib.h>

#include <unistd.h>

void butterworth_bandpass(struct data_t **data, int n)
{
    assert(cfg.highcut > 0);
    assert(cfg.lowcut > 0);
    assert(cfg.highcut > cfg.lowcut);
    assert(cfg.butter_order % 4 == 0);
    assert(cfg.samp_rate > 0);

    int order = cfg.butter_order;
    double f1 = cfg.highcut;
    double f2 = cfg.lowcut;
    double s = cfg.samp_rate;

    debug("Applying Butterworth filter (lowcut = %lf, highcut = %lf, order = %d)", f2, f1, order);

    double a = cos(M_PI * (f1 + f2) / s) / cos(M_PI * (f1 - f2) / s);
    double a2 = a * a;
    double b = tan(M_PI * (f1 - f2) / s);
    double b2 = b * b;
    double r;

    order /= 4;
    double *A =  (double *) malloc(order * sizeof(double));
    double *d1 = (double *) malloc(order * sizeof(double));
    double *d2 = (double *) malloc(order * sizeof(double));
    double *d3 = (double *) malloc(order * sizeof(double));
    double *d4 = (double *) malloc(order * sizeof(double));
    if (!A || !d1 || !d2 || !d3 || !d4)
        fatal_errno("malloc");

    double *w0 = (double *) calloc(order, sizeof(double));
    double *w1 = (double *) calloc(order, sizeof(double));
    double *w2 = (double *) calloc(order, sizeof(double));
    double *w3 = (double *) calloc(order, sizeof(double));
    double *w4 = (double *) calloc(order, sizeof(double));
    if (!w0 || !w1 || !w2 || !w3)
        fatal_errno("calloc");

    for(int i = 0; i < order; i++) {
        r = sin(M_PI*(2.0*i+1.0)/(4.0*order));
        s = b2 + 2.0*b*r + 1.0;
        A[i] = b2/s;
        d1[i] = 4.0*a*(1.0+b*r)/s;
        d2[i] = 2.0*(b2-2.0*a2-1.0)/s;
        d3[i] = 4.0*a*(1.0-b*r)/s;
        d4[i] = -(b2 - 2.0*b*r + 1.0)/s;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            double x = data[i]->d[j];

            for (int k = 0; k < order; ++k) {
                w0[k] =  x + 
                    d1[k]*w1[k] + d2[k]*w2[k] + d3[k]*w3[k] + d4[k]*w4[k];

                x = A[k] * (w0[k] - 2.0 * w2[k] + w4[k]);

                w4[k] = w3[k];
                w3[k] = w2[k];
                w2[k] = w1[k];
                w1[k] = w0[k];
            }

            data[i]->d[j] = x;
        }
    }
}

void slope(struct data_t **data, int n)
{
    assert(n > 0);
    assert(data != NULL);
    assert(data[0] != NULL);

    for (int i = 0; i < n; i++) {
        int n_eps = (cfg.plot_period / cfg.rot_eps) * cfg.samp_rate;
        if (data[i]->samp_cnt < n_eps)
            n_eps = data[i]->samp_cnt;
        double left_avg = 0;
        double right_avg = 0;
        for (int j = 0; j < n_eps; j++) {
            left_avg += data[i]->d[j] / n_eps;
            right_avg += data[i]->d[data[i]->samp_cnt - j] / n_eps;
        }

        double t1 = (double) data[i]->samp_cnt / cfg.samp_rate;
        double t = 0.0f;

        for (int j = 0; j < data[i]->samp_cnt; j++) {
            t += 1.0f / cfg.samp_rate;
            data[i]->d[j] += t * (left_avg - right_avg) / t1 - left_avg;
        }
    }
}

void scale(struct data_t **data, int n)
{
    float min = FLT_MAX;
    float max = -FLT_MAX;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            float v = data[i]->d[j];
            if (v > max) max = v;
            if (v < min) min = v;
        }
    }

    float max_hight = (2.0f * cfg.olverlap + 1) * cfg.plot_max_val;

    float dx = (max > -min) ? max : -min;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            data[i]->d[j] = (max_hight * data[i]->d[j]) / dx;
        }
    }
}
