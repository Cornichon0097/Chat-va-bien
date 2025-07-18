/**
 * \file       net.c
 * \brief      Functions dealing with network connections
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cvb/logger.h>
#include <cvb/net.h>

/**
 * \brief      Bind a socket
 *
 * The \c bind_socket() function tries to \c bind() each socket provided by
 * the address list \a rp until success.
 *
 * \param[in]  rp    The address list
 *
 * \return     The binded socket
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

/**
 * \brief      Connect a socket
 *
 * The \c connect_socket() function tries to \c connect() each socket provided
 * by the address list \a rp until success.
 *
 * \param[in]  rp    The address list
 *
 * \return     The Connected socket
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

/**
 * \brief      Fetch a socket.
 *
 * The \c fetch_socket() function tries to fetch a socket with the given
 * parameters \a host and \a service. If the \a host is NULL, then
 * \c fetch_socket() will return a socket suitable for \c clnt_accept().
 * Otherwise, the socket will be \c connect()ed directly to the \a host.
 *
 * \param[in]  host     The host
 * \param[in]  service  The service
 *
 * \return     The fetched socket
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

/**
 * \brief      Accept a new client connection
 *
 * The \c clnt_accept() function tries to \c accept() a new connection request
 * from the listening socket \a listener and return the new connected socket.
 * The argument \a listener must be a socket that has been created with
 * \c fetch_socket().
 *
 * \see fetch_socket()
 *
 * \param[in]  listener  The listening socket
 *
 * \return     The new socket on success, -1 otherwise
 */
int clnt_accept(const int listener)
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

/**
 * \brief      Send a message
 *
 * The \c send_msg() function sends up to \a size bytes from the message \a msg
 * to the socket \a sfd.
 *
 * \param[in]  sfd   The socket
 * \param[in]  msg   The message
 * \param[in]  size  The message size
 *
 * \return     the number of bytes written
 */
int send_msg(const int sfd, const void *const msg, const size_t size)
{
        int nsent;

        nsent = send(sfd, msg, size, 0);

        if (nsent == -1)
                log_error("[net] send(): %s", strerror(errno));

        return nsent;
}

/**
 * \brief      Received a message
 *
 * The \c recv_msg() function reads up to \a size bytes from the socket \a sfd
 * and stores them into the message \a msg.
 *
 * \param[in]  sfd   The socket
 * \param[out] msg   The message
 * \param[in]  size  The message size
 *
 * \return     the number of bytes read
 */
int recv_msg(const int sfd, void *const msg, const size_t size)
{
        int nread;

        nread = recv(sfd, msg, size, 0);

        if (nread == -1)
                log_error("[net] recv(): %s", strerror(errno));

        return nread;
}
