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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cvb/fdlist.h>

#include <errno.h>
#include <log.h>

/*
 * Infinite timeout for the poll() function.
 */
#define NO_TIMEOUT -1

/*
 * Try each address in the list until successfully bind a socket.
 */
static int bind_socket(const struct addrinfo *rp)
{
        int sfd, optval = 1;

        for (; rp != NULL; rp = rp->ai_next) {
                sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

                if (sfd >= 0) {
                        setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR,
                                   &optval, sizeof(int));

                        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                                return sfd;
                }

                close(sfd);
        }

        return -1;
}

/*
 * Fetch a socket and bind it.
 */
static int fetch_socket(const char *const host, const char *const service)
{
        struct addrinfo hints, *res;
        int sfd;
        int rc;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags    = AI_PASSIVE;

        rc = getaddrinfo(host, service, &hints, &res);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_fatal("getaddrinfo(): %s\n", strerror(errno));
                else
                        log_fatal("getaddrinfo(): %s\n", gai_strerror(rc));

                exit(EXIT_FAILURE);
        }

        sfd = bind_socket(res);

        freeaddrinfo(res);

        return sfd;
}

/*
 * Accept a new client connection.
 */
static int clnt_connect(int listener)
{
        struct sockaddr addr;
        socklen_t addrlen;
        char host[NI_MAXHOST], service[NI_MAXSERV];
        int sfd;
        int rc;

        sfd = accept(listener, &addr, &addrlen);

        if (sfd < 0) {
                log_warn("accept(): %s\n", strerror(errno));
                return -1;
        }

        rc = getnameinfo(&addr, addrlen, host, NI_MAXHOST,
                         service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_warn("getnameinfo(): %s\n", strerror(errno));
                else
                        log_warn("getnameinfo(): %s\n", gai_strerror(rc));
        } else
                log_info("New connection from %s:%s\n", host, service);

        return sfd;
}

/*
 * Receive a message from a connected client.
 */
static int clnt_msg(int sfd)
{
        char buf[BUFSIZ];
        int nread;

        nread = recv(sfd, buf, sizeof(buf), 0);

        if (nread <= 0) {
                if (nread < 0)
                        log_error("reecv(): %s\n", strerror(errno));
                else
                        log_info("Client disconnected\n");

                close(sfd);
                return -1;
        }

        buf[nread - 1] = '\0';
        log_info("%d bytes received: %s\n", nread, buf);

        return nread;
}

/*
 * Server's main loop.
 */
static void loop(int listener)
{
        struct fdlist fdl = FDLIST_INIT;
        nfds_t ifd;
        int ready;

        if (fdl_add(&fdl, listener) < 0) {
                perror("fdl_add()");
                exit(EXIT_FAILURE);
        }

        for (;;) {
                ready = poll(fdl_array(fdl), fdl_lenght(fdl), NO_TIMEOUT);

                if (ready < 0) {
                        perror("poll()");
                        exit(EXIT_FAILURE);
                }

                ifd = 0;

                /* TODO refactor */
                while (ready > 0) {
                        for (; ifd < fdl_lenght(fdl); ++ifd) {
                                if (fdl_get(fdl, ifd).revents == POLLIN) {
                                        if (fdl_get(fdl, ifd).fd == listener)
                                                fdl_add(&fdl, clnt_connect(fdl_get(fdl, ifd).fd));
                                        else {
                                                if (clnt_msg(fdl_get(fdl, ifd).fd) < 0)
                                                        fdl_remove(&fdl, fdl_get(fdl, ifd).fd);
                                        }

                                        --ready;
                                }
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

        if (listener < 0) {
               log_fatal("Failed to bind a socket\n");
               exit(EXIT_FAILURE);
        }

        if (listen(listener, 10) != 0) {
                log_fatal("listen(): %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        loop(listener);

        close(listener);

        return EXIT_SUCCESS;
}
