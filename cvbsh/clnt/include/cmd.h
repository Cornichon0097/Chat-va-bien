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
#ifndef CMD_H
#define CMD_H

#include <termios.h>

/*
 * Command structure initializer
 */
#define CMD_INIT {"", NULL, NULL, 0, -1, 0}

/*
 * Command buffer size
 */
#define CMD_BUFSIZ 2048

/*
 * First command line character
 */
#define CMD_LINE_CHAR_ID '/'

/*
 * Command line delimiters
 */
#define CMD_LINE_DELIM " \t\r\n\a"

/*
 * Command structure
 */
struct cmd {
        char buf[CMD_BUFSIZ];
        struct termios *tattr;
        char *ps;
        int cursor;
        int fd;
        int flags;
};

/*
 * Initialize fd for inputs
 */
int cmd_init(struct cmd *cmd, int fd, char *ps);

/*
 * Print help
 */
void cmd_help(void);

/*
 * Read command line
 */
int cmd_read(struct cmd *cmd);

/*
 * Flush command buffer
 */
void cmd_flush(struct cmd *cmd);

/*
 * Print command prompt
 */
void cmd_prompt(struct cmd *cmd);

/*
 * Restore input parameters
 */
void cmd_restore(const struct cmd *cmd);

#endif /* cmd.h */
