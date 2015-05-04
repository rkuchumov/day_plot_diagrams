#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "utils.h"
#include "params.h"
#include "format.h"
#include "plot.h"

#include "gnuplot_i.h"
#include "data.h"

int main(int argc, char *argv[]) {
    init_cfg();

    if (parse_cmd_line(argc, argv) < 0)
        exit(EXIT_FAILURE);

    parse_station_coords();

    set_tz(cfg.env_tz);

    debug("Reading CSS data (%s)", cfg.input_file);
    struct data_t **data = read_data();

    assert(data != NULL);
    assert(data[0] != NULL);

    cfg.samp_rate = data[0]->samp_rate;

    if (NULL == (cfg.date = malloc(sizeof (struct tm))))
        fatal_errno("malloc");
    localtime_r(&data[0]->time, cfg.date);
    cfg.date->tm_hour = 0;
    cfg.date->tm_min = 0;
    cfg.date->tm_sec = 0;
    time_t t0 = mktime(cfg.date);

    int n = 0;
    while(data[n] != NULL && (data[n]->time - t0) < SEC_PER_DAY)
        n++;

    if (cfg.highcut > 0 && cfg.lowcut > 0)
        butterworth_bandpass(data, n);

    slope(data, n);

    scale(data, n);

    if (cfg.output_file == NULL)
        set_output_file();

    plot(data, n);

    debug("Output file: %s", cfg.output_file);

    return 0;
}
