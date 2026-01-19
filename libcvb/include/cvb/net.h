/**
 * \file       net.h
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
#ifndef CVB_NET_H
#define CVB_NET_H

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
int net_fetch_socket(const char *host, const char *service);

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
int net_fetch_next(void);

/**
 * \brief      Accepts a new client connection.
 *
 * The \c net_accept_clnt() function tries to \c accept() a new connection
 * request from the listening socket \a listener and return the new connected
 * socket. The argument \a listener must be a socket that has been created with
 * \c net_fetch_socket().
 *
 * \see        net_fetch_socket()
 *
 * \param[in]  listener  The listening socket
 *
 * \return     The new socket on success, -1 otherwise.
 */
int net_accept_clnt(int listener);

#endif /* cvb/net.h */
