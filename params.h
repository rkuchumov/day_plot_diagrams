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

/** Config data structure */
struct cfg_t {
    unsigned debug_out : 1;
    char *cfg_file;
    char *output_file;
    char *input_file;

    unsigned data_offset;
    unsigned data_type;

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

