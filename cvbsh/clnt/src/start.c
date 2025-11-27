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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cvb/logger.h>
#include <cvb/net.h>

#include "clnt.h"

/*
 * Print usage and exit with status
 */
_Noreturn static void usage(const char *const progname, const int status)
{
        if (status != EXIT_SUCCESS) {
                fprintf(stderr, "Usage: %s [OPTIONS]... HOST PORT\n", progname);
                fprintf(stderr, "Try '%s --help' for more information\n",
                        progname);
        } else {
                printf("Usage: %s [OPTIONS]... HOST PORT\n", progname);
        }

        exit(status);
}

/*
 * Main function
 */
int main(const int argc, const char *const argv[])
{
        struct clnt clnt = {CMD_INIT, "", "", FDLIST_INIT, NULL, -1, -1};
        char service[] = "49152";

        if (argc != 3)
                usage(argv[0], EXIT_FAILURE);

        /* FIXME: log file is overrided when multiple clients are on the same
           device
         */
        clnt_set_logger(&clnt, "/tmp/cvb_clnt.log");

        if (on_exit(&clnt_cleanup, &clnt) != 0) {
                log_fatal("[start] Failed to set exit function");
                exit(EXIT_FAILURE);
        }

        if (clnt_set_handler() == -1) {
                log_fatal("[start] clnt_set_handler(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        clnt.listener = clnt_fetch_socket(service);

        if (clnt.listener == -1) {
                log_fatal("[start] Failed to fetch a socket");
                exit(EXIT_FAILURE);
        }

        clnt.srvr = net_fetch_socket(argv[1], argv[2]);

        if (clnt.srvr == -1) {
                log_fatal("[start] Failed to connect to the server");
                exit(EXIT_FAILURE);
        }

        clnt_run(&clnt);

        return EXIT_SUCCESS;
}
