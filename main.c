#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#include <math.h>

#include "utils.h"
#include "params.h"
#include "format.h"
#include "plot.h"
#include <unistd.h>

#include "gnuplot_i.h"

#include <time.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    init_cfg();

    if (parse_cmd_line(argc, argv) < 0)
        exit(EXIT_FAILURE);

    if (parse_config_file() < 0)
        exit(EXIT_FAILURE);

    set_tz(cfg.env_tz);

    debug("Opening wfdisk file (%s)", cfg.wfdisc_file);
    FILE *wfdisc_fp = fopen(cfg.wfdisc_file, "r");
    if (wfdisc_fp == NULL)
        fatal_errno("fopen");

    struct data_t **data = read_data(wfdisc_fp);

    assert(data != NULL);
    assert(data[0] != NULL);

    struct tm *ptm = localtime(&data[0]->time);
    ptm->tm_hour = 0;
    ptm->tm_min = 0;
    ptm->tm_sec = 0;
    time_t t0 = mktime(ptm);

    int n = 0;
    while(data[n] != NULL && (data[n]->time - t0) < SEC_PER_DAY)
        n++;

    float min = FLT_MAX;
    float max = -FLT_MAX;

    for (int i = 0; i < n; i++) {
        float avg = 0;
        for (int j = 0; j < data[i]->samp_cnt; j++)
            avg += data[i]->d[j] / data[i]->samp_cnt;

        for (int j = 0; j < data[i]->samp_cnt; j++) {
            data[i]->d[j] -= avg;
            float v = data[i]->d[j];

            if (v > max)
                max = v;
            if (v < min)
                min = v;
        }
    }

    float max_hight = (2.0f * cfg.olverlap + 1) * cfg.plot_max_val;

    float dx = (max > -min) ? max : -min;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            data[i]->d[j] = (max_hight * data[i]->d[j]) / dx;
        }
    }

    char **plot_data_files = (char **) malloc(sizeof(char *) * n);
    if (plot_data_files == NULL)
        fatal_errno("malloc");

    /* n = ; */
    for (int i = 0; i < n; i++) {

        plot_data_files[i] = m_mktemp();
        FILE *data_fp = fopen(plot_data_files[i], "w");
        if (data_fp == NULL)
            fatal_errno("fopen");

        debug("Temprorary data file (%s) is created", plot_data_files[i]);

        time_t dt = data[i]->time - t0;
        unsigned plot_num = dt / cfg.plot_period;

        float t = dt - plot_num * cfg.plot_period;
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            if (t > cfg.plot_period) {
                plot_num++;
                t = 0;
                fprintf(data_fp, "\n");

                if (plot_num >= SEC_PER_DAY / cfg.plot_period)
                    break;
            }

            t += 1.0f / data[i]->samp_rate;
            float v = data[i]->d[j] - 2.0f * cfg.plot_max_val * plot_num;

            fprintf(data_fp, "%f %f\n", (t / 60), v);
            /* debug("%f -> %f", t, v); */
            /* nanosleep((struct timespec[]){{0, 50000000}}, NULL); */
        }

        fclose(data_fp);
    }

    plot_init();

    plot2(plot_data_files, n);

    debug("Deleting temprorary files");
    for (int i = 0; i < n; i++) {
        if (remove(plot_data_files[i]) < 0)
            fatal_errno("remove");
    }

    return 0;
}
