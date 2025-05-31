#ifndef SRVR_H
#define SRVR_H

struct srvr {
        FILE *log;
        struct db_connect *dbc;
        struct fdlist fdl;
        int listener;
};

void loop(struct srvr *srvr);

#endif /* srvr.h */
