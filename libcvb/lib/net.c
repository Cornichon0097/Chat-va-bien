/**
 * \file       net.c
 * \brief      Functions dealing with network connections.
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
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <cvb/logger.h>

/**
 * \brief      Internal getaddrinfo().
 *
 * The \c net_getaddrinfo() function returns one or more address information
 * structures with the socket type defined as SOCK_STREAM.
 *
 * \param[in]  host     The host
 * \param[in]  service  The service
 * \param[in]  flags    The flags
 *
 * \return     The address information structure.
 */
static struct addrinfo *net_getaddrinfo(const char *const host,
                                        const char *const service,
                                        const int flags)
{
        struct addrinfo hints, *res;
        int rc;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = 0;
        hints.ai_flags = flags;

        rc = getaddrinfo(host, service, &hints, &res);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_error("[net] getaddrinfo(): %s", strerror(errno));
                else
                        log_error("[net] getaddrinfo(): %s", gai_strerror(rc));

                return NULL;
        }

        return res;
}

/**
 * \brief      Binds a socket.
 *
 * The \c net_bind_socket() function tries to \c bind() each socket provided by
 * the address list \a rp until success.
 *
 * \param[in]  rp    The address list
 *
 * \return     The binded socket.
 */
static int net_bind_socket(const struct addrinfo *rp)
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

/**
 * \brief      Connects a socket.
 *
 * The \c net_connect_socket() function tries to \c connect() each socket
 * provided by the address list \a rp until success.
 *
 * \param[in]  rp    The address list
 *
 * \return     The Connected socket.
 */
static int net_connect_socket(const struct addrinfo *rp)
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

/**
 * \brief      Fetches a socket.
 *
 * The \c net_fetch_socket() function tries to fetch a socket with the given
 * parameters \a host and \a service. If the \a host is NULL, then
 * \c net_fetch_socket() will return a socket suitable for \c net_accept_clnt().
 * Otherwise, the socket will be \c connect()ed directly to the \a host.
 *
 * \param[in]  host     The host
 * \param[in]  service  The service
 *
 * \return     The fetched socket.
 */
int net_fetch_socket(const char *const host, const char *const service)
{
        struct addrinfo *res;
        int sfd;

        assert(service != NULL);

        if (host == NULL) {
                log_debug("[net] Trying to listen on port %s", service);

                res = net_getaddrinfo(NULL, service, AI_PASSIVE);
                sfd = net_bind_socket(res);

                freeaddrinfo(res);

                if (sfd < 0) {
                        log_warn("[net] Failed to bind a socket");

                        return -1;
                }

                if (listen(sfd, 10) != 0) {
                        log_error("[net] listen(): %s", strerror(errno));

                        return -1;
                }

                log_info("[net] Listening on port %s", service);
        } else {
                log_debug("[net] Connecting to %s:%s", host, service);

                res = net_getaddrinfo(host, service, 0);
                sfd = net_connect_socket(res);

                freeaddrinfo(res);

                if (sfd < 0) {
                        log_warn("[net] Failed to connect a socket");

                        return -1;
                }

                log_info("[net] Successfuly connected to %s:%s", host, service);
        }

        return sfd;
}

/**
 * \brief      Fetches next socket.
 *
 * The \c net_fetch_next() function tries to fetch an available socket suitable
 * for \c net_accept_clnt().
 *
 * \see        net_fetch_socket()
 *
 * \return     The fetched socket.
 */
int net_fetch_next(void)
{
        char service[NI_MAXSERV];
        struct sockaddr_storage addr;
        socklen_t addrlen = sizeof(addr);
        struct addrinfo *res;
        int sfd;
        int rc;

        res = net_getaddrinfo(NULL, "0", AI_PASSIVE | AI_NUMERICHOST);
        sfd = net_bind_socket(res);

        freeaddrinfo(res);

        if (listen(sfd, 10) != 0) {
                log_error("[net] listen(): %s", strerror(errno));

                return -1;
        }

        if (getsockname(sfd, (struct sockaddr *) &addr, &addrlen) != 0) {
                log_warn("[net] getsockname(): %s", strerror(errno));

                return sfd;
        }

        rc = getnameinfo((struct sockaddr *) &addr, addrlen,
                         NULL, 0, service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_warn("[net] getnameinfo(): %s", strerror(errno));
                else
                        log_warn("[net] getnameinfo(): %s", gai_strerror(rc));
        } else {
                log_debug("[net] Listening on port %s", service);
        }

        return sfd;
}

/**
 * \brief      Accepts a new client connection.
 *
 * The \c net_accept_clnt() function tries to \c accept() a new connection
 * request from the listening socket \a listener and return the new connected
 * socket. The argument \a listener must be a socket that has been created with
 * \c net_fetch_socket().
 *
 * \see net_fetch_socket()
 *
 * \param[in]  listener  The listening socket
 *
 * \return     The new socket on success, -1 otherwise.
 */
int net_accept_clnt(const int listener)
{
        char host[NI_MAXHOST], service[NI_MAXSERV];
        struct sockaddr_storage addr;
        socklen_t addrlen;
        int sfd;
        int rc;

        sfd = accept(listener, (struct sockaddr *) &addr, &addrlen);

        if (sfd < 0) {
                log_error("[net] accept(): %s", strerror(errno));

                return -1;
        }

        rc = getnameinfo((struct sockaddr *) &addr, addrlen,
                         host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST);

        if (rc != 0) {
                if (rc == EAI_SYSTEM)
                        log_warn("[net] getnameinfo(): %s", strerror(errno));
                else
                        log_warn("[net] getnameinfo(): %s", gai_strerror(rc));
        } else {
                log_info("[net] New connection from %s:%s", host, service);
        }

        return sfd;
}
