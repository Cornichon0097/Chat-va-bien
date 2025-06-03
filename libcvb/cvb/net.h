/**
 * \file       net.h
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
#ifndef CVB_NET_H
#define CVB_NET_H

/**
 * \brief      Fetch a socket.
 *
 * The \c fetch_socket() function tries to fetch a socket with the given
 * parameters \a host and \a service. If the \a host is NULL, then
 * \c fetch_socket() will return a socket suitable for \c clnt_connect().
 * Otherwise, the socket will be \c connect()ed directly to the \a host.
 *
 * \param[in]  host     The host
 * \param[in]  service  The service
 *
 * \return     The fetched socket
 */
int fetch_socket(const char *host, const char *service);

/**
 * \brief      Accept a new client connection
 *
 * The \c clnt_connect() function tries to \c accept() a new connection request
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
int clnt_connect(int listener);

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
int send_msg(int sfd, const void *msg, size_t size);

/**
 * \brief      Read a message
 *
 * The \c read_msg() function reads up to \a size bytes from the socket \a sfd
 * and stores them into the message \a msg.
 *
 * \param[in]  sfd   The socket
 * \param[out] msg   The message
 * \param[in]  size  The message size
 *
 * \return     the number of bytes read
 */
int read_msg(int sfd, void *msg, size_t size);

#endif /* cvb/net.h */
