/*
 * CVB client.
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

#include <cvb/net.h>
#include <cvb/fdlist.h>

#include <logger.h>


/*
 * Infinite timeout for the poll() function.
 */
#define NO_TIMEOUT -1

/*
 * Client's main loop.
 */
static void loop(int server)
{
        struct fdlist fdl = FDLIST_INIT;
        struct pollfd *ifd;
        char buf[BUFSIZ];
        int nread;
        int ready;

        if (fdl_add(&fdl, STDIN_FILENO, POLLIN) < 0) {
                log_fatal("fdl_add(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (fdl_add(&fdl, server, POLLIN) < 0) {
                log_fatal("fdl_add(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        for (;;) {
                ready = poll(fdl.fds, fdl.nfds, NO_TIMEOUT);

                if (ready < 0) {
                        log_fatal("poll(): %s", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                for (ifd = fdl.fds; ready > 0; ++ifd) {
                        if (ifd->revents == POLLIN) {
                                if (ifd->fd == STDIN_FILENO) {
                                        nread = read(STDIN_FILENO, buf, sizeof(buf));

                                        if (nread > 0)
                                                send_msg(server, buf, nread);
                                }

                                --ready;
                        }
                }
        }

        fdl_destroy(&fdl);
        close(server);
}

/*
 * Client's main function.
 */
int main(const int argc, const char *const argv[])
{
        int server;

        /* TODO better log system  */
        /* TODO parse command line */

        if (argc != 3) {
                log_fatal("Usage: %s <host> <service>", argv[0]);
                exit(EXIT_FAILURE);
        }

        server = fetch_socket(argv[1], argv[2]);
        log_info("Successfuly connected to server");

        loop(server);

        return EXIT_SUCCESS;
}
