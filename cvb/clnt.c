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

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>

#include <cvb/fdlist.h>

/*
 * Infinite timeout for the poll() function.
 */
#define NO_TIMEOUT -1

/*
 * Try each address in the list until successfully connect a socket.
 */
static int connect_socket(const struct addrinfo *rp)
{
        int sfd;

        for (; rp != NULL; rp = rp->ai_next) {
                sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

                if (sfd >= 0) {
                        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
                                return sfd;
                }

                close(sfd);
        }

        return -1;
}

/*
 * Fetch a socket and connect it.
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
        hints.ai_flags    = 0;

        rc = getaddrinfo(host, service, &hints, &res);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        perror("getaddrinfo()");
                else
                        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(rc));

                exit(EXIT_FAILURE);
        }

        sfd = connect_socket(res);

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
                perror("accept()");
                return -1;
        }

        rc = getnameinfo(&addr, addrlen, host, NI_MAXHOST,
                         service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        perror("getnameinfo()");
                else
                        fprintf(stderr, "getnameinfo(): %s\n", gai_strerror(rc));
        } else
                printf("New connection from %s:%s\n", host, service);

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
                        perror("reecv()");
                else
                        printf("Client disconnected\n");

                close(sfd);
                return -1;
        }

        buf[nread - 1] = '\0';
        printf("%d bytes received: %s\n", nread, buf);

        return nread;
}

/*
 * Client's main loop.
 */
static void loop(int server)
{
        struct fdlist fdl = FDLIST_INIT;
        char buf[BUFSIZ];
        nfds_t ifd;
        int ready;

        if (fdl_add(&fdl, STDIN_FILENO) < 0) {
                perror("fdl_add()");
                exit(EXIT_FAILURE);
        }

        if (fdl_add(&fdl, server) < 0) {
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
                                        if (fdl_get(fdl, ifd).fd == STDIN_FILENO) {
                                                read(STDIN_FILENO, buf, sizeof(buf));
                                                fdl_get(fdl, 1).events = POLLOUT; /* warning, change the fixed idf */
                                        } else {
                                        }

                                        --ready;
                                }
                        }
                }
        }

        fdl_destroy(&fdl);
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
                fprintf(stderr, "Usage: %s host port\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        server = fetch_socket(argv[1], argv[2]);

        if (server < 0) {
               fprintf(stderr, "Failed to connect a socket\n");
               exit(EXIT_FAILURE);
        }

        loop(server);

        close(server);

        return EXIT_SUCCESS;
}
