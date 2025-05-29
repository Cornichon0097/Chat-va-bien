#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cvb/net.h>

#include <logger.h>

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

                        close(sfd);
                }
        }

        return -1;
}

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

                        close(sfd);
                }
        }

        return -1;
}

/*
 * Fetch a socket and bind it.
 */
int fetch_socket(const char *const host, const char *const service)
{
        struct addrinfo hints, *res;
        int sfd;
        int rc;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;

        if (host == NULL)
                hints.ai_flags = AI_PASSIVE;

        rc = getaddrinfo(host, service, &hints, &res);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_fatal("getaddrinfo(): %s", strerror(errno));
                else
                        log_fatal("getaddrinfo(): %s", gai_strerror(rc));

                exit(EXIT_FAILURE);
        }

        if (host == NULL) {
                sfd = bind_socket(res);

                if (listen(sfd, 10) != 0) {
                        log_error("listen(): %s", strerror(errno));
                }
        } else
                sfd = connect_socket(res);

        if (sfd < 0) {
               log_fatal("Failed to bind a socket");
               exit(EXIT_FAILURE);
        }

        freeaddrinfo(res);

        return sfd;
}

/*
 * Accept a new client connection.
 */
int clnt_connect(const int listener)
{
        struct sockaddr addr;
        socklen_t addrlen;
        char host[NI_MAXHOST], service[NI_MAXSERV];
        int sfd;
        int rc;

        sfd = accept(listener, &addr, &addrlen);

        if (sfd < 0) {
                log_error("accept(): %s", strerror(errno));
                return -1;
        }

        rc = getnameinfo(&addr, addrlen, host, NI_MAXHOST,
                         service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_warn("getnameinfo(): %s", strerror(errno));
                else
                        log_warn("getnameinfo(): %s", gai_strerror(rc));
        } else
                log_info("New connection from %s:%s", host, service);

        return sfd;
}

int send_msg(const int sfd, const void *const msg, const size_t size)
{
        int nsent;

        nsent = send(sfd, msg, size, 0);

        if (nsent < 0) {
                log_error("send(): %s", strerror(errno));
                return -1;
        }

        return nsent;
}

/*
 * Receive a message from a connected client.
 */
int read_msg(const int sfd, void *const msg, const size_t size)
{
        int nread;

        nread = recv(sfd, msg, size, 0);

        if (nread < 0)
                log_error("recv(): %s", strerror(errno));

        if (nread == 0) {
                log_info("Client disconnected");
                close(sfd);
        }

        return nread;
}

