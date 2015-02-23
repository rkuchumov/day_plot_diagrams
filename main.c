#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "utils.h"
#include "params.h"
#include "format.h"
#include <unistd.h>

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

    while(!feof(fp)) {
        float value = read_flt(fp);
        debug("%f", value);

        sleep(1);
    }

    return 0;
}
