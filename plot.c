#include "plot.h"
#include "gnuplot_i.h"

#define CMD_LEN 90000
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

    sprintf(cmd, "set style line 2 lc rgb \'#a00000\'");
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set xrange[%d:%d]", 0, cfg.plot_period / 60);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set notitle");
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set nokey");
    gnuplot_cmd(h, cmd);

    return 1;
}

int plot2(char **files, int plot_cnt)
{
    debug("Drawing plot");

    sprintf(cmd, "set output \"%s\"", cfg.output_file);
    gnuplot_cmd(h, cmd);

    /* sprintf(cmd, "set yrange[%f:%f]", min, max); */
    /* gnuplot_cmd(h, cmd); */

    /* sprintf(cmd, "set multiplot layout %d,1", plot_cnt + 1); */
    /* gnuplot_cmd(h, cmd); */

    char *e = cmd;
    e += sprintf(e, "plot ");
    for (int i = 0; i < plot_cnt; i++) {
        e += sprintf(e, "\"%s\" with lines ls %d, ", files[i], 1 + i % 2);
    }
    gnuplot_cmd(h, cmd);

    gnuplot_close(h);

    return 1;
}
