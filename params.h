#ifndef PARAMS_H
#define PARAMS_H

#define DFT_DEBUG_OUT 0

#define DFT_STA_FILE "stations.txt"
#define DFT_OUTPUT_FILE NULL
#define DFT_INPUT_FILE "data.wfdisk"

#define DFT_CHANNEL "MHZ"

#define DFT_SAMPLING_RATE 50
#define DFT_PLOT_PERIOD 900
#define DFT_OVERLAP 5

#define DFT_PLOT_WIDTH 900
#define DFT_PLOT_HEIGHT 1200
#define DFT_PLOT_LINE_COLOR "#000000"
#define DFT_PLOT_MAX_VAL 100.0f
#define DFT_YTICS_FONT_SIZE 10
#define DFT_ROT_EPS 50
#define DFT_STATION_NAME NULL
#define DFT_DATE NULL

#define DFT_LOWCUT -1.0f
#define DFT_HIGHCUT -1.0f
#define DFT_BUTTER_ORDER 6

#define DFT_TIMEZONE "UTC"

/** Config data structure */
struct cfg_t {
    unsigned debug_out : 1;
    char *output_file;
    char *input_file;
    char *channel;

    unsigned olverlap;
    unsigned samp_rate;
    unsigned plot_period;
    char *station_name;
    struct tm *date;

    unsigned plot_width;
    unsigned plot_height;
    char *plot_line_color;
    float plot_max_val;
    unsigned ytics_font_size;
    unsigned rot_eps;

    double lowcut;
    double highcut;
    unsigned butter_order;

    char *env_tz;
} cfg;

struct sta_t {
    char *name;
    char *coords;
} *stations;

/** Sets default values to cfg structre fields
 * @see cfg_t
 * @return -1 on error, 1 on success
 */
void init_cfg();

void parse_station_coords();

/** Parses command line params into cfg structure
 * @return -1 on error, 1 on success
 */
int parse_cmd_line(int argc, char *argv[]);

void set_input_file(char *path);
void set_output_file();

#endif /* end of include guard: PARAMS_H */

