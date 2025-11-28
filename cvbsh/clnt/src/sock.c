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
#include <assert.h>
#include <bits/stdint-intn.h>
#include <stdio.h>
#include <string.h>

#include <cvb/logger.h>
#include <cvb/msg.h>

#include "sock.h"

/*
 * Send authentification request
 */
int send_auth_request(const int srvr, const char *const uname,
                      const char *const psswd)
{
        int rc;

        assert(uname != NULL);

        if (psswd == NULL)
                rc = msg_send_code(srvr, MSG_CODE_SEND_NO_AUTH);
        else
                rc = msg_send_code(srvr, MSG_CODE_SEND_AUTH);

        if (rc <= 0)
                return rc;

        rc = msg_send_text(srvr, uname, strlen(uname));

        if (rc <= 0)
                return rc;

        if (psswd != NULL)
                msg_send_text(srvr, psswd, strlen(psswd));

        if (msg_recv_code(srvr) != MSG_CODE_RECV_AUTH)
                return -1;

        return msg_recv_code(srvr);
}

/*
 * Send connection request
 */
int8_t send_connect_request(int srvr, const char *name)
{
        char buf[MSG_BUFSIZ];
        int rc;

        assert(name != NULL);

        rc = msg_send_code(srvr, MSG_CODE_DM_REQUEST);

        if (rc <= 0)
                return rc;

        rc = msg_send_text(srvr, name, strlen(name));

        if (rc <= 0)
                return rc;

        if (msg_recv_code(srvr) != MSG_CODE_DM_STATUS)
                return -1;

        rc = msg_recv_text(srvr, buf);

        if (rc <= 0)
                return rc;

        if (strcmp(buf, name) != 0)
                return -1;

        return msg_recv_code(srvr);
}

/*
 * Send a message
 */
static int send_msg(const int sfd, const int8_t code, const char *const msg)
{
        short size = (short) strlen(msg);
        int rc;

        if (size == 0)
                return 0;

        rc = msg_send_code(sfd, code);

        if (rc <= 0)
                return rc;

        return msg_send_text(sfd, msg, size);
}

/*
 * Send public message
 */
int send_public_message(const int srvr, const char *const msg)
{
        assert(msg != NULL);

        return send_msg(srvr, MSG_CODE_SEND_PUBLIC, msg);
}

/*
 * Send private message
 */
int send_private_message(const int clnt, const char *const msg)
{
        assert(msg != NULL);

        return send_msg(clnt, MSG_CODE_DM, msg);
}
