#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>

#include "utils.h"
#include "params.h"
#include "format.h"
#include "plot.h"
#include <unistd.h>

#include "gnuplot_i.h"

int main(int argc, char *argv[]) {
    init_cfg();

    if (parse_cmd_line(argc, argv) < 0)
        exit(EXIT_FAILURE);

    if (parse_config_file() < 0)
        exit(EXIT_FAILURE);

    debug("Opening %s", cfg.input_file);
    FILE *fp = fopen(cfg.input_file, "r");
    if (fp == NULL)
        fatal_errno("fopen");

    if (parse_header(fp) < 0)
        exit(EXIT_FAILURE);

    long spl_cnt = samples_cnt(fp);
    debug("Samples count in file= %ld", spl_cnt);

    long spl_per_plot = cfg.sampling_rate * cfg.plot_period / cfg.avg_cnt;
    debug("Samples per plot = %ld", spl_per_plot);

    int plot_num = spl_cnt / (cfg.sampling_rate * cfg.plot_period);
    debug("Number of plots = %d", plot_num);

    float *data = malloc(sizeof(float) * spl_cnt / cfg.avg_cnt);
    if (data == NULL)
        fatal_errno("malloc");


    float max = FLT_MIN;
    float min = FLT_MAX;

    for (int i = 0; i < spl_cnt / cfg.avg_cnt; i++) {
        float avg = 0;
        for (unsigned j = 0; j < cfg.avg_cnt; j++) {
            /* if (feof(fp)) */
                /* goto out; */

            avg += (read_flt(fp) / cfg.avg_cnt);
        }

        if (max < avg)
            max = avg;
        if (min > avg)
            min = avg;

        data[i] = avg;
    }

    char *tmp_file = m_mktemp();
    FILE *tmp_fp = fopen(tmp_file, "w");
    if (tmp_fp == NULL)
        fatal_errno("fopen");

    float dt = (float) cfg.avg_cnt / (cfg.sampling_rate * 60.0f);
    for (int i = 0; i < spl_per_plot; i++) {
        fprintf(tmp_fp, "%f ", (float) i * dt);

        for (int j = 0; j < plot_num; j++)
            fprintf(tmp_fp, "%f ", data[i + j * (spl_per_plot)]);
        fprintf(tmp_fp, "\n");
    }

    fflush(tmp_fp);
    fclose(tmp_fp);

    plot_init();

    plot(tmp_file, plot_num, min, max);

    return 0;
}
