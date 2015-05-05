#include "utils.h"
#include "params.h"
#include "data.h"
#include "iir.h"

#include <float.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

void butterworth_bandpass(struct data_t **data, int n)
{
    assert(cfg.highcut > 0);
    assert(cfg.lowcut > 0);
    assert(cfg.highcut > cfg.lowcut);
    assert(cfg.butter_order > 0);
    assert(cfg.samp_rate > 0);

    if (cfg.lowcut > cfg.highcut)
        fatal("Incorrect cutoff frequencies");

    double nyquist = cfg.samp_rate / 2.0f;
    if (cfg.lowcut >= nyquist || cfg.highcut >= nyquist) 
        fatal("Cutoff frequency(ies) should be less than %0.2f (half the sample rate)",
                nyquist);

    debug("Applying Butterworth filter (lowcut = %lf, highcut = %lf, order = %d)",
            cfg.lowcut, cfg.highcut, cfg.butter_order);
    double fl = cfg.lowcut / nyquist;
    double fh = cfg.highcut / nyquist;
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
    debug("gain = %lf", 1 / gain);
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

            xv[l - 1] = gain * data[i]->d[j];

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
    samp_t min = SAMP_MAX;
    samp_t max = SAMP_MIN;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            samp_t v = data[i]->d[j];
            if (v > max) max = v;
            if (v < min) min = v;
        }
    }

    double max_hight = (2.0f * cfg.olverlap + 1) * cfg.plot_max_val;

    samp_t dx = (max > -min) ? max : -min;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            data[i]->d[j] = (max_hight * data[i]->d[j]) / dx;
        }
    }
}
