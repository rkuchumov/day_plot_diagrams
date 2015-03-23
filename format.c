#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>

#include <libgen.h>

#include <unistd.h>

#include "utils.h"
#include "format.h"
#include "params.h"

#define FILES_CNT 40

struct wf_t {
    FILE *fp;

    char chan[9];
    float time;
    int nsamp;
    float samprate;
    char dir[65];
    char dfile[33];
    int foff;
};

struct data_t **read_data(FILE *wfdisc_file);

struct wf_t *next_file(FILE *wfdisc_fp);
void free_wf(struct wf_t *wf);
char *read_wfdisc_line(FILE *wfdisc_fp);
void parse_wfdisc_line(char *line, struct wf_t *wf);
void open_data_file(struct wf_t *wf);
int skip_data_file(struct wf_t *wf);

struct data_t **read_data(FILE *wfdisc_fp)
{
    struct data_t **ret;
    unsigned data_size = sizeof(struct data_t *) * FILES_CNT;
    ret = (struct data_t **) malloc(data_size);
    if (ret == NULL)
        fatal_errno("malloc");

    struct wf_t *wf;
    int i = 0;
    while ((wf = next_file(wfdisc_fp)) != NULL) {
        if (i > FILES_CNT) {
            data_size += sizeof(struct data_t *) * FILES_CNT;
            ret = (struct data_t **) realloc(ret, data_size);
            if (ret == NULL)
                fatal_errno("malloc");
        }

        ret[i] = (struct data_t *) malloc(sizeof(struct data_t));
        if (ret[i] == NULL)
            fatal_errno("malloc");

        ret[i]->samp_rate = wf->samprate;
        ret[i]->samp_cnt = wf->nsamp;
        ret[i]->time = (time_t) wf->time;

        ret[i]->d = (float *) malloc(sizeof(float) * wf->nsamp);
        if (ret[i]->d == NULL)
            fatal_errno("malloc");

        for (int j = 0; j < wf->nsamp; j++) {
            if (feof(wf->fp))
                fatal("unexpected EOF in %s", wf->dfile);

            ret[i]->d[j] = read_int(wf->fp);
        }

        fclose(wf->fp);
        i++;
    }

    ret[i] = NULL;

    return ret;
}

struct wf_t *next_file(FILE *wfdisc_fp) 
{
    assert(wfdisc_fp != NULL);
    assert(cfg.is_inited);

    struct wf_t *wf = (struct wf_t *) malloc(sizeof(struct wf_t));
    if (wf == NULL)
        fatal_errno("malloc");

    do {
        char *line = read_wfdisc_line(wfdisc_fp);
        if (line == NULL)
            return NULL;

        parse_wfdisc_line(line, wf);

        free(line);

    } while (skip_data_file(wf));

    open_data_file(wf);
    assert(wf->fp != NULL);

    if (fseek(wf->fp, wf->foff, SEEK_SET) < 0)
        fatal_errno("fseek");

    return wf;
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
        &wf->nsamp,
        &wf->samprate,
        wf->dir,
        wf->dfile,
        &wf->foff
            );

    if (rc != 7) /* number of assigned values in sscanf */
        fatal("error while reading wfsisc file");

#if 0
    debug("wfdisc line read (%zd assigned values)", rc);
    debug("chan \t= %s", wf->chan);
    debug("time \t= %17.5f", wf->time);
    debug("nsamp \t= %d", wf->nsamp);
    debug("samprate \t= %17.5f", wf->samprate);
    debug("dir \t= %s", wf->dir);
    debug("dfile \t= %s", wf->dfile);
    debug("foff \t= %d", wf->foff);
#endif
}

void open_data_file(struct wf_t *wf)
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

    wf->fp = fopen(path, "r");
    if (wf->fp == NULL)
        fatal_errno("fopen");
}

int skip_data_file(struct wf_t *wf)
{
    assert(cfg.is_inited);
    assert(wf != NULL);

    if (strcmp(wf->chan, cfg.channel) == 0)
        return false;

    return true;
}

void free_wf(struct wf_t *wf)
{
    if (wf->fp != NULL)
        fclose(wf->fp);
    wf->fp = NULL;
}
