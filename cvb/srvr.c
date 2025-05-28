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
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <net.h>
#include <logger.h>

#include <cvb/fdlist.h>

/*
 * Infinite timeout for the poll() function.
 */
#define NO_TIMEOUT -1

/*
 * Server's main loop.
 */
static void loop(int listener)
{
        struct fdlist fdl = FDLIST_INIT;
        struct pollfd *ifd;
        char buf[BUFSIZ];
        int ready;

        if (fdl_add(&fdl, listener, POLLIN) < 0) {
                log_fatal("fdl_add(): %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        for (;;) {
                ready = poll(fdl.fds, fdl.nfds, NO_TIMEOUT);

                if (ready < 0) {
                        log_fatal("poll(): %s\n", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                /* TODO refactor */
                for (ifd = fdl.fds; ready > 0; ++ifd) {
                        if (ifd->revents == POLLIN) {
                                if (ifd->fd == listener)
                                        fdl_add(&fdl, clnt_connect(ifd->fd), POLLIN);
                                else {
                                        if (read_msg(ifd->fd, buf, BUFSIZ) < 0)
                                                fdl_remove(&fdl, ifd->fd);
                                }

                                --ready;
                        }
                }
        }

        fdl_destroy(&fdl);
}

/*
 * Server's main function.
 */
int main(const int argc, const char *const argv[])
{
        int listener;

        /* TODO better log system  */
        /* TODO parse command line */

        if (argc != 2) {
                log_fatal("Usage: %s port\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        listener = fetch_socket(NULL, argv[1]);

        loop(listener);

        close(listener);

        return EXIT_SUCCESS;
}
