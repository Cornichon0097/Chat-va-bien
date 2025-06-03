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
                        log_error("[net] getaddrinfo(): %s", strerror(errno));
                else
                        log_error("[net] getaddrinfo(): %s", gai_strerror(rc));

                return -1;
        }

        if (host == NULL) {
                sfd = bind_socket(res);

                if (listen(sfd, 10) != 0) {
                        log_error("[net] listen(): %s", strerror(errno));
                }
        } else {
                log_debug("[net] Attempt connection to %s:%s", host, service);
                sfd = connect_socket(res);
        }

        if (sfd < 0) {
               log_error("[net] Failed to bind a socket");
               return -1;
        }

        if (host == NULL)
                log_info("[net] Listening on port %s", service);
        else
                log_info("[net] Successfuly connected to %s:%s", host, service);

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
                log_error("[net] accept(): %s", strerror(errno));
                return -1;
        }

        rc = getnameinfo(&addr, addrlen, host, NI_MAXHOST,
                         service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_warn("[net] getnameinfo(): %s", strerror(errno));
                else
                        log_warn("[net] getnameinfo(): %s", gai_strerror(rc));
        } else
                log_info("[net] New connection from %s:%s", host, service);

        return sfd;
}

int send_msg(const int sfd, const void *const msg, const size_t size)
{
        int nsent;

        nsent = send(sfd, msg, size, 0);

        if (nsent == -1)
                log_error("[net] send(): %s", strerror(errno));

        return nsent;
}

/*
 * Receive a message from a connected client.
 */
int read_msg(const int sfd, void *const msg, const size_t size)
{
        int nread;

        nread = recv(sfd, msg, size, 0);

        if (nread == -1)
                log_error("[net] recv(): %s", strerror(errno));

        return nread;
}
