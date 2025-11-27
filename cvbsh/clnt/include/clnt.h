/*
 * CVB client
 *
 * Copyright (c) 2025 Antoni Blanche
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CLNT_H
#define CLNT_H

#include <stdio.h>

#include <cvb/fdlist.h>
#include <cvb/msg.h>

#include "cmd.h"

/*
 * Client structure
 */
struct clnt {
        struct cmd cmd;
        char uname[MSG_BUFSIZ];
        char name_last_msg[MSG_BUFSIZ];
        struct fdlist fdl;
        FILE *log_file;
        int srvr;
        int listener;
};

/*
 * Initialize client logger
 */
void clnt_set_logger(struct clnt *clnt, const char *pathname);

/*
 * Update client signal handler
 */
int clnt_set_handler(void);

/*
 * Fetch the first available socket
 */
int clnt_fetch_socket(char *service);

/*
 * Client loop
 */
void clnt_run(struct clnt *clnt);

/*
 * Client destroyer
 */
void clnt_cleanup(int status, void *arg);

#endif /* clnt.h */
