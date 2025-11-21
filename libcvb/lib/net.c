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
        struct addrinfo hints, *res;
        int sfd;
        int rc;

        assert(service != NULL);

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
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
                log_debug("[net] Trying to listen on port %s", service);

                sfd = net_bind_socket(res);

                if (listen(sfd, 10) != 0)
                        log_warn("[net] listen(): %s", strerror(errno));
        } else {
                log_debug("[net] Attempt connection to %s:%s", host, service);

                sfd = net_connect_socket(res);
        }

        freeaddrinfo(res);

        if (sfd < 0) {
                if (host == NULL)
                        log_warn("[net] Failed to bind a socket");
                else
                        log_warn("[net] Failed to connect a socket");

                return -1;
        }

        if (host == NULL)
                log_info("[net] Listening on port %s", service);
        else
                log_info("[net] Successfuly connected to %s:%s", host, service);

        return sfd;
}

/**
 * \brief      Fetches next socket.
 *
 * The \c net_fetch_next() function tries to fetch a socket with the very after
 * port number \a service.
 *
 * \see        net_fetch_socket()
 *
 * \param[in]  host     The host
 * \param      service  The service
 *
 * \return     The fetched socket.
 */
int net_fetch_next(const char *const host, char *const service)
{
        assert (service != NULL);

        ++service[4];

        if (service[4] == ':') {
                service[4] = '0';
                ++service[3];
        }

        if (service[3] == ':') {
                service[3] = '0';
                ++service[2];
        }

        if (service[2] == ':')
                return -1;

        return net_fetch_socket(host, service);
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
        } else {
                log_info("[net] New connection from %s:%s", host, service);
        }

        return sfd;
}
