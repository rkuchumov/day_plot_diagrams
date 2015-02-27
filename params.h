#ifndef PARAMS_H
#define PARAMS_H

#include <argp.h>

#define FLOAT 1

#define DFT_DEBUG_OUT 0
#define DFT_CFG_FILE "config.ini"
#define DFT_OUTPUT_FILE "output"
#define DFT_INPUT_FILE NULL
#define DFT_DATA_OFFSET 512
#define DFT_DATA_TYPE FLOAT
#define DFT_SAMPLING_RATE 50
#define DFT_PLOT_PERIOD 3600 /* 1 hr */
#define DFT_AVERAGE_SIZE 500
#define DFT_PLOT_WIDTH 900
#define DFT_PLOT_HEIGHT 250
#define DFT_PLOT_LINE_COLOR "#0060ad"

/** Config data structure */
struct cfg_t {
    unsigned debug_out : 1;
    char *cfg_file;
    char *output_file;
    char *input_file;

    unsigned data_offset;
    unsigned data_type;
    unsigned sampling_rate;

    unsigned plot_period;
    unsigned avg_cnt; // TODO rename
    unsigned plot_width;
    unsigned plot_height;
    char *plot_line_color;

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

