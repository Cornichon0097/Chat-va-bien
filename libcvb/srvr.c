#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <stdio.h>

#include <cvb/fdlist.h>

#define NO_TIMEOUT -1

/*
 * Try each address in the list until a socket successfully bind
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
 *
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
                        perror("getaddrinfo()");
                else
                        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(rc));

                exit(EXIT_FAILURE);
        }

        sfd = bind_socket(res);

        freeaddrinfo(res);

        return sfd;
}

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
 *
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
                /* printf("Data (fdl lenght %lu)\n", fdl_lenght(fdl)); */

                if (ready < 0) {
                        perror("poll()");
                        exit(EXIT_FAILURE);
                }

                ifd = 0;

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
 *
 */
int main(const int argc, const char *const argv[])
{
        int listener;

        /* TODO better log system  */
        /* TODO parse command line */

        if (argc != 2) {
                fprintf(stderr, "Usage: %s port\n", argv[0]);
                exit(EXIT_FAILURE);
        }

        listener = fetch_socket(NULL, argv[1]);

        if (listener < 0) {
               fprintf(stderr, "Failed to bind a socket\n");
               exit(EXIT_FAILURE);
        }

        if (listen(listener, 10) != 0) {
                perror("listen()");
                exit(EXIT_FAILURE);
        }

        loop(listener);

        close(listener);

        return EXIT_SUCCESS;
}
