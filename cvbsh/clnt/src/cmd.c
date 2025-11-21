#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cvb/logger.h>
#include <cvb/msg.h>

#include "cmd.h"

#define CMD_LINE_CHAR_ID ':'

#define CMD_LINE_DELIM " \t\r\n\a"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "cmd.h"

#define CLEAR_STRING "\033[2J\033[H"

static int cmd_set_non_blocking(const int fd)
{
        int flags = fcntl(fd, F_GETFL, 0);

        if (flags == -1)
                return -1;

        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* static int cmd_set_input_modes(const int fd)
{
        struct termios tattr;

        if (tcgetattr(fd, &tattr) == -1)
                return -1;

        tattr.c_lflag = tattr.c_lflag & ~(ICANON | ECHO);
        tattr.c_cc[VMIN] = 1;
        tattr.c_cc[VTIME] = 0;

        return tcsetattr(fd, TCSAFLUSH, &tattr);
} */

int cmd_init(struct cmd *const cmd, const int fd)
{
        assert(cmd != NULL);

        cmd->fd = fd;
        cmd->cursor = 0;

        if (!isatty(fd))
                return -1;

        if (cmd_set_non_blocking(fd) == -1)
                return -1;

        /* return cmd_set_input_modes(fd); */
        return 0;
}

int cmd_read(struct cmd *const cmd)
{
        int nread;

        assert(cmd != NULL);

        while ((nread = read(cmd->fd, cmd->buf + cmd->cursor, 1)) == 1) {
                switch (cmd->buf[cmd->cursor]) {
                case '\n':
                        cmd->buf[cmd->cursor] = '\0';

                        return '\n';

                case 127:
                case '\b':
                        if (cmd->cursor > 0) {
                                --cmd->cursor;
                                cmd->buf[cmd->cursor] = '\0';
                        }
                        break;

                default:
                        ++cmd->cursor;
                        break;
                }
        }

        if (nread == -1)
                return -1;

        return cmd->buf[cmd->cursor];
}

void cmd_prompt(const char *const ps)
{
        printf("%s> ", ps);
        fflush(stdout);
}

/* static int cmd_help(void)
{
        printf("List of commands:\n");
        printf("\n");
        printf("MESSAGE       Send public MESSAGE to all users\n");
        printf(":dm USER MESSAGE  Send direct MESSAGE to USER\n");
        printf(":ft PATHNAME      Transfer file PATHNAME to server\n");
        printf(":quit        Exit chat app\n");
        printf(":help [COMMAND]   cmd this help\n");

        return 0;
} */
