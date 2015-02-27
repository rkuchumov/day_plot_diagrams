#include "plot.h"
#include "gnuplot_i.h"

#define CMD_LEN 130
char cmd[CMD_LEN];

gnuplot_ctrl *h;

int plot_init()
{
    h = gnuplot_init();
    if (h == NULL)
        fatal("gnuplot init failed");

    sprintf(cmd, "set terminal png size %u,%u", 
            cfg.plot_width, cfg.plot_height);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set style line 1 lc rgb \'%s\'",
            cfg.plot_line_color);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set xrange[%d:%d]", 0, cfg.plot_period / 60);
    gnuplot_cmd(h, cmd);

    return 1;
}

int plot(char *data_file, int plot_cnt, float min, float max)
{
    sprintf(cmd, "set output \"%s\"", cfg.output_file);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set yrange[%f:%f]", min, max);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set multiplot layout %d,1", plot_cnt);
    gnuplot_cmd(h, cmd);

    for (int i = 2; i < plot_cnt + 2; i++) {
        sprintf(cmd, "plot \"%s\" using 1:%d with lines ls 1", data_file, i);
        gnuplot_cmd(h, cmd);
    }

    gnuplot_close(h);

    return 1;
}
