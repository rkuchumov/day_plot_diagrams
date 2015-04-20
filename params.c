#include "params.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include <getopt.h>
#include <libgen.h>

void usage(char **argv) {
    printf("Usage: %s [OPTION...] FILE\n", basename(argv[0]));
    printf("%s -- CSS data visuzlization\n", basename(argv[0]));
    printf("\n");
    printf("  -c, --channel=CODE         Channel code to process data from\n");
    printf("  -d, --order=NUM            Butterworth bandpass filter order\n");
    printf("  -H, --highcut=FREQUENCY    Butterworth bandpass higher cutoff frequency\n");
    printf("  -L, --lowcut=FREQUENCY     Butterworth bandpass lower cutoff frequency\n");
    printf("  -o, --output=FILE          Output FILE name\n");
    printf("  -p, --olverlap=NUM         Number of adjacent plots that can be olverlaped\n");
    printf("  -v, --verbose              Produce verbose output\n");
    printf("  -h, --help, --usage        Give this help list\n");
    printf("  -V, --version              Print program version\n");
    printf("\n");
    printf("Mandatory or optional arguments to long options are also mandatory or optional\n");
    printf("for any corresponding short options.\n");
    printf("\n");
    printf("Report bugs to <kuchumovri@gmail.com>.\n");

    exit(EXIT_SUCCESS);
}

void short_usage(char **argv)
{
    printf("Usage: %s [-vV] [-c CODE] [-d NUM] [-H FREQUENCY] [-L FREQUENCY]\n", basename(argv[0]));
    printf("         [-o FILE] [-p NUM] [--channel=CODE] [--order=NUM]\n");
    printf("         [--highcut=FREQUENCY] [--lowcut=FREQUENCY] [--output=FILE]\n");
    printf("         [--olverlap=NUM] [--verbose] [--help] [--usage] [--version] FILE\n");
    printf("\n");
    printf("Try `%s --help' or `%s --usage` for more information.\n",
            basename(argv[0]), basename(argv[0]));

    exit(EXIT_SUCCESS);
}

void version()
{
    printf("geodiagmas 1.0");

    exit(EXIT_SUCCESS);
}

void init_cfg()
{
    cfg.debug_out = DFT_DEBUG_OUT;
    cfg.output_file = DFT_OUTPUT_FILE;
    cfg.cfg_file = DFT_CFG_FILE;

    cfg.wfdisc_file = DFT_WFDISC_FILE;
    cfg.channel = DFT_CHANNEL;

    cfg.samp_rate = DFT_SAMPLING_RATE;
    cfg.plot_period = DFT_PLOT_PERIOD;
    cfg.plot_width = DFT_PLOT_WIDTH;
    cfg.plot_height = DFT_PLOT_HEIGHT;
    cfg.plot_line_color = DFT_PLOT_LINE_COLOR;

    cfg.env_tz = DFT_TIMEZONE;

    cfg.olverlap = DFT_OVERLAP;
    cfg.plot_max_val = DFT_PLOT_MAX_VAL;
    cfg.ytics_font_size = DFT_YTICS_FONT_SIZE;
    cfg.rot_eps = DFT_ROT_EPS;
    cfg.date = DFT_DATE;
    cfg.station_name = DFT_STATION_NAME;

    cfg.lowcut = DFT_LOWCUT;
    cfg.highcut = DFT_HIGHCUT;
    cfg.butter_order = DFT_BUTTER_ORDER;

    cfg.is_inited = true;
}

int parse_config_file()
{
    /* TODO  */
    return 1;
}

int parse_cmd_line(int argc, char *argv[])
{
    int c;

    static struct option long_options[] =
    {
        {"verbose", no_argument,       0, 'v'},
        {"help",    no_argument,       0, 'h'},
        {"usage",   no_argument,       0, 'h'},
        {"version", no_argument,       0, 'V'},
        {"channel", required_argument, 0, 'c'},
        {"order",   required_argument, 0, 'd'},
        {"highcut", required_argument, 0, 'h'},
        {"lowcut",  required_argument, 0, 'l'},
        {"output",  required_argument, 0, 'o'},
        {"overlap", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    double d;
    int v;

    while (1) {
        int id = 0;

        if ((c = getopt_long (argc, argv, "vVhp:o:L:H:d:c:", 
                        long_options, &id)) == -1)
        {
            break;
        }

        switch (c) {
        case 'v':
            cfg.debug_out = true;
            break;
        case 'o':
            cfg.output_file = optarg;
            break;
        case 's':
            cfg.cfg_file = optarg;
            break;
        case 'c':
            cfg.channel = optarg;
            break;
        case 'p':
            v = atoi(optarg);
            if (v > 0)
                cfg.olverlap = v;
            break;
        case 'L':
            d = strtod(optarg, NULL);
            if (d > 0)
                cfg.lowcut = d;
            break;
        case 'H':
            d = strtod(optarg, NULL);
            if (d > 0)
                cfg.highcut = d;
            break;
        case 'd':
            v = atoi(optarg);
            if (v > 0)
                cfg.butter_order = v;
            break;
        case 'V':
            version();
        case 'h':
            usage(argv);
        default:
            short_usage(argv);
        }
    }

    if (optind < argc && optind + 1 >= argc)
        cfg.wfdisc_file = argv[optind];
    else
        short_usage(argv);

    if (cfg.lowcut * cfg.highcut < 0.0f)
        fatal("You should specify both high and low cutoff frequencies");

    if (cfg.lowcut > cfg.highcut)
        fatal("Incorrect cutoff frequency");

    if (cfg.butter_order % 4 != 0)
        fatal("Butterworth filter order should be 4,8,12,16,...");

#if 0
    debug("Command line agruments:");
    debug("debug_out = %s", cfg.debug_out ? "yes":"no");
    debug("wfdisc file = %s", cfg.wfdisc_file);
    debug("channel code = %s", cfg.channel);
    debug("config file = %s", cfg.cfg_file);
    debug("output file = %s", cfg.output_file);
    debug("lowcut freq = %lf", cfg.lowcut);
    debug("highcut freq = %lf", cfg.highcut);
    debug("filter order = %d", cfg.butter_order);
#endif

    return 1;
}
