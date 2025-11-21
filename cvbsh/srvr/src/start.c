/*
 * CVB server.
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cvb/logger.h>
#include <cvb/net.h>

#include "srvr.h"

_Noreturn static void usage(const char *const progname, const int status)
{
        if (status != EXIT_SUCCESS) {
                fprintf(stderr, "Usage: %s [OPTIONS]... PORT\n", progname);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        progname);
        } else {
                printf("Usage: %s [OPTIONS]... PORT\n", progname);
        }

        exit(status);
}

int main(const int argc, const char *const argv[])
{
        struct srvr srvr = {FDLIST_INIT, NULL, -1};

        if (argc != 2)
                usage(argv[0], EXIT_FAILURE);

        srvr_set_logger(&srvr, "/tmp/cvb_srvr.log");

        if (on_exit(&srvr_cleanup, &srvr) != 0) {
                log_fatal("[srvr] Failed to set exit function");
                exit(EXIT_FAILURE);
        }

        if (srvr_set_handler() == -1) {
                log_fatal("[srvr] sigaction(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        /* srvr.dbc = db_init("mongodb://cornichon:vinaigre@localhost:27017/", DB_URI);

        if (srvr.dbc == NULL) {
                log_fatal("[srvr] Failed to connect to database");
                exit(EXIT_FAILURE);
        } */

        srvr.listener = net_fetch_socket(NULL, argv[1]);

        if (srvr.listener == -1) {
                log_fatal("[srvr] Failed to fetch a socket");
                exit(EXIT_FAILURE);
        }

        srvr_run(&srvr);

        return EXIT_SUCCESS;
}
