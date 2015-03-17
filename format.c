#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <stdint.h>
#include <assert.h>

#include <libgen.h>

#include "utils.h"
#include "format.h"
#include "params.h"

union u4 {
    float f;
    int32_t i;
    uint32_t u;

    uint8_t data[4];
};

struct wf_t {
    char chan[9];
    float time;
    int nstamp;
    float samprate;
    char dir[65];
    char dfile[33];
    int foff;
};

char *read_wfdisc_line(FILE *wfdisc_fp);
void parse_wfdisc_line(char *line, struct wf_t *wf);
FILE *open_data_file(struct wf_t *wf);
int skip_data_file(struct wf_t *wf);

FILE *next_file(FILE *wfdisc_fp) 
{
    assert(wfdisc_fp != NULL);
    assert(cfg.is_inited);

    struct wf_t wf;

    do {
        char *line = read_wfdisc_line(wfdisc_fp);
        if (line == NULL)
            return NULL;

        parse_wfdisc_line(line, &wf);

        free(line);

    } while (skip_data_file(&wf));

    FILE *data_fp = open_data_file(&wf);
    assert(data_fp != NULL);

    if (fseek(data_fp, wf.foff, SEEK_SET) < 0)
        fatal_errno("fseek");

    return data_fp;
};

char *read_wfdisc_line(FILE *wfdisc_fp)
{
    assert(wfdisc_fp != NULL);
    assert(cfg.is_inited);

	char *line = NULL;
	size_t len = 0;
	ssize_t rc;
	if ((rc = getline(&line, &len, wfdisc_fp)) <= 0) {
        if (feof(wfdisc_fp))
            return NULL;

        fatal_errno("getline");
    }

    return line;
}

void parse_wfdisc_line(char *line, struct wf_t *wf)
{
    assert(line != NULL);
    assert(wf != NULL);
    assert(cfg.is_inited);

    size_t rc = sscanf(line, 
            "%*s " /* station code */
            "%s "  /* channel code */
            "%f "  /* epoch time of first sample in file */
            "%*d " /* waveform identifier */
            "%*d " /* channel identifier */
            "%*d " /* julian date */
            "%*f " /* time +(nsamp -1 )/samles */
            "%d "  /* number of samples */
            "%f "  /* sampling rate in samples/sec */
            "%*f " /* nominal calibration */
            "%*f " /* nominal calibration period */
            "%*s " /* instrument code */
            "%*s " /* indexing method */
            "%*s " /* numeric storage */
            "%*s " /* clipped flag */
            "%s "  /* directory */
            "%s "  /* data file */
            "%d "  /* byte offset of data seg ment within file */
            "%*d " /* comment identifier */
            "%*s ", /* load date */
        wf->chan,
        &wf->time,
        &wf->nstamp,
        &wf->samprate,
        wf->dir,
        wf->dfile,
        &wf->foff
            );

    if (rc != 7) /* number of assigned values in sscanf */
        fatal("error while reading wfsisc file");

    debug("wfdisc line read (%zd assigned values)", rc);
    debug("chan \t= %s", wf->chan);
    debug("time \t= %17.5f", wf->time);
    debug("nstamp \t= %d", wf->nstamp);
    debug("samprate \t= %17.5f", wf->samprate);
    debug("dir \t= %s", wf->dir);
    debug("dfile \t= %s", wf->dfile);
    debug("foff \t= %d", wf->foff);
}

FILE *open_data_file(struct wf_t *wf)
{
    assert(cfg.is_inited);
    assert(wf != NULL);
    assert(wf->dir != NULL);
    assert(wf->dfile != NULL);

    char path[PATH_MAX] = {0};
    char *e = path;

    char tmp[PATH_MAX] = {0};

    strcpy(tmp, cfg.wfdisc_file);
    e += sprintf(e, "%s/", dirname(tmp));

    strcpy(tmp, wf->dir);
    e += sprintf(e, "%s/", dirname(tmp));

    strcpy(tmp, wf->dfile);
    e += sprintf(e, "%s", basename(tmp));

    debug("Opening data file (%s)", path);

    FILE *data_fp = fopen(path, "r");
    if (data_fp == NULL)
        fatal_errno("fopen");

    return data_fp;
}

int skip_data_file(struct wf_t *wf)
{
    assert(cfg.is_inited);
    assert(wf != NULL);

    if (strcmp(wf->chan, cfg.channel) == 0)
        return false;

    return true;
}

float read_flt(FILE *fp) 
{
    union u4 flt_u;
    uint8_t d[4];

    fread(d, 1, 4, fp);

    /* flt_u.data[0] = d[3]; */
    /* flt_u.data[1] = d[2]; */
    /* flt_u.data[2] = d[1]; */
    /* flt_u.data[3] = d[0]; */

    flt_u.data[0] = d[0];
    flt_u.data[1] = d[1];
    flt_u.data[2] = d[2];
    flt_u.data[3] = d[3];

    /* debug("%#02x %#02x %#02x %#02x --> %f",  */
    /*         flt_u.data[0],  */
    /*         flt_u.data[1],  */
    /*         flt_u.data[2],  */
    /*         flt_u.data[3],  */
    /*         flt_u.f); */

    return flt_u.f;
}
