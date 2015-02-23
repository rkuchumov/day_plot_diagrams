#include "params.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

const char *argp_program_version =
  "main 1.0";

const char *argp_program_bug_address =
  "<kuchumovri@gmail.com>";

static char doc[] =
  "main -- program description";

static char args_doc[] = "FILE";

static struct argp_option options[] = {
  {"verbose",  'v', 0,      0,  "Produce verbose output"},
  {"debug",    'd', 0,      OPTION_ALIAS},
  {"output",   'o', "FILE", 0, "Output FILE name"},
  {"config",   'c', "FILE", 0, "Config FILE name (default: config.ini)"},
  {0}
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct cfg_t *cfg = state->input;

    switch (key) {
        case 'v': case 'd':
            cfg->debug_out = true;
            break;
        case 'o':
            cfg->output_file = arg;
            break;
        case 'c':
            cfg->cfg_file = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 1)
                /* Too many arguments. */
                argp_usage (state);

            cfg->input_file = arg;

            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1)
                /* Not enough arguments. */
                argp_usage (state);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

void init_cfg() {
    cfg.debug_out = DFT_DEBUG_OUT;
    cfg.output_file = DFT_OUTPUT_FILE;
    cfg.cfg_file = DFT_CFG_FILE;

    cfg.data_offset = DFT_DATA_OFFSET;
    cfg.data_type = DFT_DATA_TYPE;

    cfg.is_inited = true;
}

int parse_config_file() {
    /* TODO  */
    return 1;
}

int parse_cmd_line(int argc, char *argv[]) {
    argp_parse (&argp, argc, argv, 0, 0, &cfg);

    debug("Command line agruments:");
    debug("debug_out = %s", cfg.debug_out ? "yes":"no");
    debug("input file = %s", cfg.input_file);
    debug("config file = %s", cfg.cfg_file);
    debug("output file = %s", cfg.output_file);

    return 1;
}
