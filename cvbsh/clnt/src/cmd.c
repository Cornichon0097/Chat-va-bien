/*
 * CVB client commands
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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cvb/logger.h>
#include <cvb/msg.h>

#include "cmd.h"

/*
 * Set input as non-blocking
 */
static int cmd_set_non_blocking(struct cmd *const cmd)
{
        cmd->flags = fcntl(cmd->fd, F_GETFL, 0);

        if (cmd->flags == -1)
                return -1;

        return fcntl(cmd->fd, F_SETFL, cmd->flags | O_NONBLOCK);
}

/*
 * Set input modes
 */
static int cmd_set_input_modes(struct cmd *const cmd)
{
        struct termios tattr;

        if (tcgetattr(cmd->fd, &tattr) == -1)
                return -1;

        memccpy(cmd->tattr, &tattr, 1, sizeof(struct termios));

        tattr.c_lflag = tattr.c_lflag & ~(ICANON | ECHO);
        tattr.c_cc[VMIN] = 1;
        tattr.c_cc[VTIME] = 0;

        return tcsetattr(cmd->fd, TCSAFLUSH, &tattr);
}

/*
 * Initialize fd for inputs
 */
int cmd_init(struct cmd *const cmd, const int fd, char *const ps)
{
        assert(cmd != NULL);

        cmd->tattr = (struct termios *) malloc(sizeof(struct termios));

        if (cmd->tattr == NULL)
                return -1;

        cmd->ps = ps;
        cmd->cursor = 0;
        cmd->fd = fd;

        if (!isatty(fd))
                return -1;

        if (cmd_set_non_blocking(cmd) == -1)
                return -1;

        return cmd_set_input_modes(cmd);
}

/*
 * Delete last command line character
 */
static void cmd_del(struct cmd *const cmd)
{
        assert(cmd != NULL);

        if (cmd->cursor > 0) {
                --cmd->cursor;
                cmd->buf[cmd->cursor] = '\0';

                printf("\033[1D");
                printf("\033[K");
        }
}

/*
 * Get stroke key
 */
static void cmd_getkey(struct cmd *const cmd, const char c)
{
        assert(cmd != NULL);

        if (cmd->cursor < CMD_BUFSIZ - 1) {
                cmd->buf[cmd->cursor] = c;
                ++cmd->cursor;
                cmd->buf[cmd->cursor] = '\0';

                putchar(c);
        }
}

/*
 * Print help
 */
void cmd_help(void)
{
        printf("\nList of commands:\n");
        printf("\n");
        printf(">MESSAGE           Send public MESSAGE to all users\n");
        printf(">/dm USER MESSAGE  Send direct MESSAGE to USER\n");
        /* printf(">/ft PATHNAME      Transfer file PATHNAME to server\n"); */
        printf(">/help             Display this help\n");
        printf(">/quit             Exit chat app\n");
}

/*
 * Read command line
 */
int cmd_read(struct cmd *const cmd)
{
        char c;

        assert(cmd != NULL);

        while ((c = getchar()) != EOF) {
                if (c == '\n') {
                        if (cmd->cursor < CMD_BUFSIZ - 1) {
                                cmd->buf[cmd->cursor] = '\0';
                        }

                        return '\n';
                }

                if ((c == 127) || (c == '\b'))
                        cmd_del(cmd);
                else
                        cmd_getkey(cmd, c);
        }

        if (c == EOF)
                return -1;

        return cmd->buf[cmd->cursor];
}

/*
 * Parse command line
 */
char **cmd_parse(struct cmd *cmd)
{
        char **args = NULL;

        if (cmd->buf[0] != CMD_LINE_CHAR_ID)
                return NULL;

        args = (char **) malloc(3 * sizeof(char *));

        if (args == NULL) {
                perror("malloc()");
                exit(EXIT_FAILURE);
        }

        args[0] = strtok(cmd->buf, CMD_LINE_DELIM);
        args[1] = strtok(NULL, CMD_LINE_DELIM);
        args[2] = strtok(NULL, CMD_LINE_DELIM);

        return args;
}

/*
 * Flush command buffer
 */
void cmd_flush(struct cmd *const cmd)
{
        cmd->cursor = 0;
        cmd->buf[0] = '\0';
}

/*
 * Print command prompt
 */
void cmd_prompt(struct cmd *const cmd)
{
        assert(cmd != NULL);

        cmd_flush(cmd);

        printf("\e\[1m%s\e\[0m> %s", cmd->ps, cmd->buf);
        fflush(stdout);
}

/*
 * Restore input parameters
 */
void cmd_restore(const struct cmd *const cmd)
{
        assert(cmd != NULL);

        if (cmd->tattr) {
                fcntl(cmd->fd, F_SETFL, cmd->flags);
                tcsetattr(cmd->fd, TCSAFLUSH, cmd->tattr);
                free(cmd->tattr);
        }
}
