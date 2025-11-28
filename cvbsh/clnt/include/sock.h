/*
 * CVB client communication
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
#ifndef SOCK_H
#define SOCK_H

#include <sys/types.h>

/*
 * Send authentification request
 */
int send_auth_request(int srvr, const char *uname, const char *psswd);

/*
 * Send connection request
 */
int8_t send_connect_request(int srvr, const char *name);

/*
 * Send public message
 */
int send_public_message(int srvr, const char *msg);

/*
 * Send private message
 */
int send_private_message(int clnt, const char *msg);

/*
 * Receive IPv4 address
 */
short recv_ipv4_addr(int srvr, void *ip);

/*
 * Receive IPv6 address
 */
short recv_ipv6_addr(int srvr, void *ip);

#endif /* sock.h */
