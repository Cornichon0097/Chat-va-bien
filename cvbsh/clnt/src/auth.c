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
#include "sock.h"

/*
 * Get information for authentification
 */
static int auth_read_line(const char *const field, char *buf, const size_t size)
{
        printf("%s: ", field);
        fflush(stdout);

        if (fgets(buf, size, stdin) == NULL)
                return -1;

        buf[strlen(buf) - 1] = '\0';

        return 0;
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
                auth_read_line("username", uname, size);
                auth_read_line("password", pwd, MSG_BUFSIZ);

                log_debug("[auth] Send logging request as %s", uname);

                if (*pwd == '\0')
                        auth = send_auth_request(srvr, uname, NULL);
                else
                        auth = send_auth_request(srvr, uname, pwd);

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
