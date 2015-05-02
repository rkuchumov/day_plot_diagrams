#include "utils.h"
#include "params.h"
#include "data.h"
#include "iir.h"

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
    assert(cfg.butter_order > 0);
    assert(cfg.samp_rate > 0);

    debug("Applying Butterworth filter (lowcut = %lf, highcut = %lf, order = %d)",
            cfg.lowcut, cfg.highcut, cfg.butter_order);
    double fl = 2.0f * cfg.lowcut / cfg.samp_rate;
    double fh = 2.0f * cfg.highcut / cfg.samp_rate;
    double order = cfg.butter_order;

    double *b = dcof_bwbp(order, fl, fh);
    int *a = ccof_bwbp(order);
    double gain = sf_bwbp(order, fl, fh);

    int l = 2 * order + 1;
    double *xv = calloc(sizeof(double), l);
    if (xv == NULL)
        fatal_errno("calloc");
    double *yv = calloc(sizeof(double), l);
    if (yv == NULL)
        fatal_errno("calloc");

#if 0
    debug("gain = %lf", gain);
    for (int k = 0; k < l; k++) {
        debug("%d * x%d", a[l - k - 1], k);
    }
    for (int k = 0; k < l - 1; k++) {
        debug("%lf * y%d", -b[l - k - 1], k);
    }
#endif

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            for (int k = 1; k < l; k++) {
                xv[k - 1] = xv[k];
                yv[k - 1] = yv[k];
            }

            xv[l - 1] = data[i]->d[j] * gain;

            yv[l - 1] = 0;
            for (int k = 0; k < l; k++)
                yv[l - 1] += xv[k] * ((double) a[l - k - 1]);
            for (int k = 0; k < l - 1; k++)
                yv[l - 1] -= yv[k] * b[l - k - 1];

            data[i]->d[j] = yv[l - 1];
        }
    }

    free(b);
    free(a);
    free(xv);
    free(yv);
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
            right_avg += data[i]->d[data[i]->samp_cnt - j - 1] / n_eps;
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
/*  */
/*     float avg = 0.0f; */
/*     int n_eps = 3; */
/*     for (int i = 0; i < n; i++) { */
/*         for (int j = 0; j < data[i]->samp_cnt; j++) { */
/*             if (j % n_eps == 0) { */
/*                 if (avg > max) max = avg; */
/*                 if (avg < min) min = avg; */
/*                 avg = data[i]->d[j] / n_eps; */
/*             } else { */
/*                 avg += data[i]->d[j] / n_eps; */
/*             } */
/*         } */
/*     } */
/*  */

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
