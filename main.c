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
#include "data.h"

#include <time.h>
#include <assert.h>

#define DATE_SIZE 60

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

    cfg.samp_rate = data[0]->samp_rate;

    struct tm *ptm = localtime(&data[0]->time);
    ptm->tm_hour = 0;
    ptm->tm_min = 0;
    ptm->tm_sec = 0;
    time_t t0 = mktime(ptm);

    cfg.date = malloc(DATE_SIZE);
    if (cfg.date == NULL)
        fatal_errno("malloc");
    strftime(cfg.date, DATE_SIZE, "%F", ptm);

    int n = 0;
    while(data[n] != NULL && (data[n]->time - t0) < SEC_PER_DAY)
        n++;

    if (cfg.highcut > 0 && cfg.lowcut > 0)
        butterworth_bandpass(data, n);

    slope(data, n);

    scale(data, n);

    plot(data, n);

    return 0;
}
