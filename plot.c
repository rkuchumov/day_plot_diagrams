#include "plot.h"

#include "params.h"
#include "utils.h"
#include "format.h"
#include "gnuplot_i.h"

#include <stdlib.h>
#include <assert.h>

#include <time.h>

char **creat_plot_files(struct data_t **data, int n);
gnuplot_ctrl *init();
void draw(gnuplot_ctrl *h, char **files, int plot_cnt);

#define CMD_LEN 90000
char cmd[CMD_LEN];

gnuplot_ctrl *init()
{
    gnuplot_ctrl *h = gnuplot_init();
    if (h == NULL)
        fatal("gnuplot init failed");

    sprintf(cmd, "set terminal png size %u,%u", 
            cfg.plot_width, cfg.plot_height);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set style line 1 lc rgb \'%s\'", cfg.plot_line_color);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set style line 2 lc rgb \'#a00000\'");
    gnuplot_cmd(h, cmd);
    sprintf(cmd, "set style line 3 lc rgb \'#a000a0\'");
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set xrange[%d:%d]", 0, cfg.plot_period / 60);
    gnuplot_cmd(h, cmd);


    float min = -2.0f * (SEC_PER_DAY / cfg.plot_period) * cfg.plot_max_val;
    sprintf(cmd, "set yrange[%f:%f]", min, cfg.plot_max_val);
    gnuplot_cmd(h, cmd);

    time_t t = 0;

    char *e = cmd;
    e += sprintf(cmd, "set ytics(\"00:00\" 0");
    for (unsigned i = 1; i < SEC_PER_DAY / cfg.plot_period; i++) {
        t += cfg.plot_period;

        if (i % 2)
            continue;

        struct tm *ptm = localtime(&t);

        float pos = -2.0f * i * cfg.plot_max_val;
        e += sprintf(e, ", \"%02d:%02d\" %f", ptm->tm_hour, ptm->tm_min, pos);
    }
    sprintf(e, ") font \",%d\"", cfg.ytics_font_size);
    gnuplot_cmd(h, cmd);

    e = cmd;
    t = cfg.plot_period;
    struct tm *ptm = localtime(&t);
    e += sprintf(cmd, "set y2tics(\"%02d:%02d\" 0", ptm->tm_hour, ptm->tm_min);
    for (unsigned i = 1; i < SEC_PER_DAY / cfg.plot_period; i++) {
        t += cfg.plot_period;

        if (i % 2)
            continue;

        ptm = localtime(&t);

        float pos = -2.0f * i * cfg.plot_max_val;
        e += sprintf(e, ", \"%02d:%02d\" %f", ptm->tm_hour, ptm->tm_min, pos);
    }
    sprintf(e, ") font \",%d\"", cfg.ytics_font_size);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set notitle");
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set nokey");
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set tmargin 4");
    gnuplot_cmd(h, cmd);

    assert(cfg.date != NULL);
    char date[DATE_LEN];
    strftime(date, DATE_LEN, "%Y-%m-%d", cfg.date);
    sprintf(cmd, "set label \"Date: %s\" left at graph 0, screen 0.99", date);
    gnuplot_cmd(h, cmd);

    assert(cfg.station_name != NULL);
    int found = -1;
    for (int i = 0; stations[i].name != NULL; i++) {
        if (strcmp(stations[i].name, cfg.station_name) == 0) {
            found = i;
            break;
        }
    }

    if (found < 0) {
        sprintf(cmd, "set label \"Station: %s\" left at graph 0, screen 0.97",
                cfg.station_name);
        gnuplot_cmd(h, cmd);
    } else {
        sprintf(cmd, "set label \"Station: %s (%s)\" left at graph 0, screen 0.97", 
                cfg.station_name, stations[found].coords);
        gnuplot_cmd(h, cmd);
    }

    assert(cfg.channel != NULL);
    sprintf(cmd, "set label \"Channel: %s\" left at graph 0, screen 0.95", cfg.channel);
    gnuplot_cmd(h, cmd);

    sprintf(cmd, "set label \"Overlap: %d\" left at graph 0.5, screen 0.99", cfg.olverlap);
    gnuplot_cmd(h, cmd);

    if (cfg.highcut > 0 && cfg.lowcut > 0) {
        sprintf(cmd, "set label \"Filter lowcut/highcut freq.: %0.2f/%0.2f \" left at graph 0.5, screen 0.97",
                cfg.lowcut, cfg.highcut);
        gnuplot_cmd(h, cmd);

        sprintf(cmd, "set label \"Filter order: %d \" left at graph 0.5, screen 0.95",
                cfg.butter_order);
        gnuplot_cmd(h, cmd);
    } else {
        sprintf(cmd, "set label \"No Filter\" left at graph 0.5, screen 0.97");
        gnuplot_cmd(h, cmd);
    }

    return h;
}

void draw(gnuplot_ctrl *h, char **files, int plot_cnt)
{
    debug("Drawing plot");

    sprintf(cmd, "set output \"%s\"", cfg.output_file);
    gnuplot_cmd(h, cmd);

    char *e = cmd;
    e += sprintf(e, "plot ");
    for (int i = 0; i < plot_cnt; i++) {
        /* e += sprintf(e, "\"%s\" with lines ls %d, ", files[i], 2 + i % 2); */
        e += sprintf(e, "\"%s\" with lines ls %d, ", files[i], 1);
    }
    gnuplot_cmd(h, cmd);

    gnuplot_close(h);
}

char **creat_plot_files(struct data_t **data, int n)
{
    debug("Creating temprorary files");
    time_t t0 = day_start(data[0]->time);

    char **plot_data_files = (char **) malloc(sizeof(char *) * n);
    if (plot_data_files == NULL)
        fatal_errno("malloc");

    for (int i = 0; i < n; i++) {
        plot_data_files[i] = m_mktemp();
        FILE *data_fp = fopen(plot_data_files[i], "w");
        if (data_fp == NULL)
            fatal_errno("fopen");

        time_t dt = data[i]->time - t0;
        unsigned plot_num = dt / cfg.plot_period;

        double t = (double) dt - plot_num * cfg.plot_period;
        for (int j = 0; j < data[i]->samp_cnt; j++) {
            if (t > cfg.plot_period) {
                plot_num++;
                t = 0;
                fprintf(data_fp, "\n");

                if (plot_num >= SEC_PER_DAY / cfg.plot_period)
                    break;
            }

            t += 1.0f / cfg.samp_rate;
            float v = data[i]->d[j] - 2.0f * cfg.plot_max_val * plot_num;

            fprintf(data_fp, "%lf %f\n", (t / 60), v);
            /* debug("%f -> %f", t, v); */
            /* nanosleep((struct timespec[]){{0, 50000000}}, NULL); */
        }

        fclose(data_fp);
    }

    return plot_data_files;
}

void plot(struct data_t **data, int n) {
    char **tmp_files = creat_plot_files(data, n);
    assert(tmp_files != NULL);

    gnuplot_ctrl *h = init();

    draw(h, tmp_files, n);

    debug("Deleting temprorary files");
    for (int i = 0; i < n; i++) {
        if (remove(tmp_files[i]) < 0)
            fatal_errno("remove");

        free(tmp_files[i]);
    }
    free(tmp_files);
}
