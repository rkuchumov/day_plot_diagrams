#ifndef PARAMS_H
#define PARAMS_H

#include <argp.h>

#define FLOAT 1

#define DFT_DEBUG_OUT 0

#define DFT_CFG_FILE "config.ini"
#define DFT_OUTPUT_FILE "output.png"
#define DFT_INPUT_FILE "data.wfdisk"

#define DFT_WFDISC_FILE "data.wfdisc"

#define DFT_CHANNEL "MHZ"

#define DFT_DATA_OFFSET 512
#define DFT_DATA_TYPE FLOAT
#define DFT_SAMPLING_RATE 50
#define DFT_PLOT_PERIOD 900

#define DFT_AVERAGE_SIZE 50
#define DFT_PLOT_WIDTH 900
#define DFT_PLOT_HEIGHT 1200
#define DFT_PLOT_LINE_COLOR "#000000"

#define DFT_OVERLAP 5
#define DFT_PLOT_MAX_VAL 100.0f
#define DFT_YTICS_FONT_SIZE 10
#define DFT_ROT_EPS 50
#define DFT_STATION_NAME NULL
#define DFT_DATE NULL

#define DFT_LOWCUT -1.0f
#define DFT_HIGHCUT -1.0f
#define DFT_BUTTER_ORDER 4

#define DFT_TIMEZONE "UTC"

/** Config data structure */
struct cfg_t {
    unsigned debug_out : 1;
    char *cfg_file;
    char *output_file;
    char *input_file;

    char *wfdisc_file;
    char *channel;

    unsigned data_offset;
    unsigned data_type;
    unsigned samp_rate;

    unsigned plot_period;
    unsigned avg_cnt; // TODO rename
    unsigned plot_width;
    unsigned plot_height;
    char *plot_line_color;

    double lowcut;
    double highcut;
    unsigned butter_order;

    char *env_tz;

    unsigned olverlap;
    float plot_max_val;
    unsigned ytics_font_size;
    unsigned rot_eps;

    char *date;
    char *station_name;

    unsigned is_inited : 1;
} cfg;

/** Sets default values to cfg structre fields
 * @see cfg_t
 * @return -1 on error, 1 on success
 */
void init_cfg();

/** Opens config file, parses it into cfg structure
 * @return -1 on error, 1 on success
 */
int parse_config_file();

/** Parses command line params into cfg structure
 * @return -1 on error, 1 on success
 */
int parse_cmd_line(int argc, char *argv[]);

#endif /* end of include guard: PARAMS_H */

