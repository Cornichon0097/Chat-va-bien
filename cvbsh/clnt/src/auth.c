/*
 * CVB client authentification
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
#include <stdio.h>
#include <string.h>

#include <cvb/logger.h>
#include <cvb/msg.h>

#include "auth.h"

/*
 * Get information for authentification
 */
static int auth_get(const char *const field, char *buf, const size_t size)
{
        printf("%s: ", field);
        fflush(stdout);

        if (fgets(buf, size, stdin) == NULL)
                return -1;

        buf[strlen(buf) - 1] = '\0';

        return 0;
}

/*
 * Send request to the server
 */
static int auth_send(const int srvr, const char *const uname,
                     const char *const pwd)
{
        int rc;

        if (pwd == NULL)
                rc = msg_send_code(srvr, MSG_CODE_SEND_NO_AUTH);
        else
                rc = msg_send_code(srvr, MSG_CODE_SEND_AUTH);

        if (rc <= 0)
                return rc;

        msg_send_text(srvr, uname, strlen(uname));

        if (pwd != NULL)
                msg_send_text(srvr, pwd, strlen(pwd));

        if (msg_recv_code(srvr) != MSG_CODE_RECV_AUTH)
                return -1;

        return msg_recv_code(srvr);
}

/*
 * Send authentification request
 */
int auth_request(const int srvr, char *const uname, const size_t size)
{
        char pwd[MSG_BUFSIZ];
        int auth = -1;

        assert(uname != NULL);

        while (auth != 0) {
                auth_get("username", uname, size);
                auth_get("password", pwd, MSG_BUFSIZ);

                log_debug("[auth] Send logging request as %s", uname);

                if (*pwd == '\0')
                        auth = auth_send(srvr, uname, NULL);
                else
                        auth = auth_send(srvr, uname, pwd);

                if (auth == 1)
                        fprintf(stderr, "Wrong username or password\n");
                else if (auth == 2)
                        fprintf(stderr, "Username already taken\n");
                else if (auth == -1)
                        log_error("[auth] Connection to server lost");
        }

        log_info("[auth] Logged in as %s", uname);

        return 0;
}
