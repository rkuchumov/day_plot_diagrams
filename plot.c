#include "plot.h"
#include "gnuplot_i.h"

#define CMD_LEN 130

gnuplot_ctrl *h;

int plot_init()
{
    h = gnuplot_init();
    if (h == NULL)
        fatal("gnuplot init failed");

    char cmd[CMD_LEN];
	sprintf(cmd, "set terminal png size %u,%u", 
	        cfg.plot_width, cfg.plot_height);
    gnuplot_cmd(h, cmd);

	sprintf(cmd, "set style line 1 lc rgb \'%s\'",
	        cfg.plot_line_color);
    gnuplot_cmd(h, cmd);

    gnuplot_cmd(h, "set output \"sine.png\"");
    /* gnuplot_cmd(h, "plot \"tmp1.dat\" with lines ls 1"); */
    /* gnuplot_close(h); */

    return 1;
}
