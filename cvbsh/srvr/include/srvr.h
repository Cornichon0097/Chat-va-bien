#ifndef SRVR_H
#define SRVR_H

#include <stdio.h>

#include <cvb/fdlist.h>
#include <cvb/fdmap.h>

struct srvr {
        struct fdlist fdl;
        struct fdmap fdm;
        FILE *log;
        /* struct db_connect *dbc; */
        int listener;
};

void srvr_set_logger(struct srvr *srvr, const char *pathname);

int srvr_set_handler(void);

void srvr_run(struct srvr *srvr);

void srvr_cleanup(int status, void *arg);

#endif /* srvr.h */
