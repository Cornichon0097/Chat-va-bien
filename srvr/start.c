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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <cvb/net.h>
#include <cvb/fdlist.h>

#include <logger.h>

#include <srvr.h>
#include <db.h>

#define FILENAME "/tmp/cvb_srvr.log"

static void cleanup(__attribute__((unused)) int status, void *arg)
{
        struct srvr *srvr = (struct srvr *) arg;

        log_debug("[srvr] Cleaning up");

        if (srvr->log != NULL)
                fclose(srvr->log);

        if (srvr->dbc != NULL)
                db_close(&(srvr->dbc));

        if (srvr->fdl.fds != NULL)
                fdl_destroy(&(srvr->fdl));

        if (srvr->listener > -1)
                close(srvr->listener);
}

static void handler(__attribute__((unused)) int signal)
{
        log_info("[srvr] Clean up and shut down");
        exit(EXIT_SUCCESS);
}

static void usage(const char *const argv0)
{
        fprintf(stderr, "Usage: %s <port>\n", argv0);
}

static int set_handler(void)
{
        struct sigaction act;

        log_debug("[srvr] Set signal handler");

        act.sa_handler = &handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        return sigaction(SIGINT, &act, NULL);
}

static void set_logger(struct srvr *const srvr, const char *const pathname)
{
        srvr->log = fopen(pathname, "w");

        if (srvr->log != NULL)
                log_callback(&file_callback, srvr->log, LOG_DEBUG);
        else
                log_error("[srvr] Faile to open file %s", FILENAME);

        log_debug("[srvr] Logging level set to %s", log_level(LOG_ERROR));
}

/*
 * Server's main function.
 */
int main(const int argc, const char *const argv[])
{
        struct srvr srvr = {NULL, NULL, FDLIST_INIT, -1};
        int rc;

        if (argc != 2) {
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }

        set_logger(&srvr, FILENAME);

        rc = on_exit(&cleanup, &srvr);

        if (rc != 0) {
                log_fatal("[srvr] Failed to set exit function");
                exit(EXIT_FAILURE);
        }

        rc = set_handler();

        if (rc == -1) {
                log_fatal("[srvr] sigaction(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        srvr.dbc = db_init("mongodb://cornichon:vinaigre@localhost:27017/", DB_URI);
        srvr.listener = fetch_socket(NULL, argv[1]);

        loop(&srvr);

        return EXIT_SUCCESS;
}
