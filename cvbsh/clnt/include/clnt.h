#ifndef CLNT_H
#define CLNT_H

#include <stdio.h>

#include <cvb/fdlist.h>
#include <cvb/msg.h>

#include "cmd.h"

struct clnt {
        struct cmd cmd;
        char uname[MSG_BUFSIZ];
        char name_last_msg[MSG_BUFSIZ];
        struct fdlist fdl;
        FILE *log_file;
        int srvr;
        int listener;
};

void clnt_set_logger(struct clnt *clnt, const char *pathname);

int clnt_set_handler(void);

int clnt_fetch_socket(char *service);

void clnt_run(struct clnt *clnt);

void clnt_cleanup(int status, void *arg);

#endif /* clnt.h */
