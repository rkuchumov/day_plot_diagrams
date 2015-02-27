#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "params.h"
#include "format.h"
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
    debug("Samples count = %ld", spl_cnt);

    long spl_per_plot = cfg.sampling_rate * cfg.plot_period;
    debug("Samples per plot = %ld", spl_per_plot);

    int plot_num = spl_cnt / spl_per_plot;
    debug("Number of plots = %d", plot_num);

    FILE *tmp1 = fopen("tmp1.dat", "w+");
    if (tmp1 == NULL)
        fatal_errno("fopen");

    float dt = (float) cfg.avg_cnt / (cfg.sampling_rate * 60.0f);

    float avg;
    for (unsigned i = 0; i < spl_per_plot / cfg.avg_cnt; i++) {
        avg = 0;
        for (unsigned j = 0; j < cfg.avg_cnt; j++) {
            if (feof(fp))
                goto out;

            avg += (read_flt(fp) / cfg.avg_cnt);
        }

        fprintf(tmp1, "%f %f\n", (float) i * dt , avg);
    }

out:
    fflush(tmp1);
    fclose(tmp1);

    /* TODO */
    gnuplot_ctrl *h;
    h = gnuplot_init();
    if (h == NULL)
        fatal("gnuplot init failed");

    char cmd[150];
	sprintf(cmd, "set terminal png size %u,%u", 
	        cfg.plot_width, cfg.plot_height);
    gnuplot_cmd(h, cmd);

	sprintf(cmd, "set style line 1 lc rgb \'%s\'",
	        cfg.plot_line_color);
    gnuplot_cmd(h, cmd);

    gnuplot_cmd(h, "set output \"sine.png\"");
    gnuplot_cmd(h, "plot \"tmp1.dat\" with lines ls 1");
    gnuplot_close(h);


    return 0;
}
