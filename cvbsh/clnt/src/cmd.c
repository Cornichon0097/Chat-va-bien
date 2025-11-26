#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cvb/logger.h>
#include <cvb/msg.h>

#include "cmd.h"

static int cmd_set_non_blocking(struct cmd *const cmd)
{
        cmd->flags = fcntl(cmd->fd, F_GETFL, 0);

        if (cmd->flags == -1)
                return -1;

        return fcntl(cmd->fd, F_SETFL, cmd->flags | O_NONBLOCK);
}

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

void cmd_help(void)
{
        printf("\nList of commands:\n");
        printf("\n");
        printf("MESSAGE           Send public MESSAGE to all users\n");
        /* printf("/dm USER MESSAGE  Send direct MESSAGE to USER\n"); */
        /* printf("/ft PATHNAME      Transfer file PATHNAME to server\n"); */
        printf("/quit             Exit chat app\n");
        printf("/exit             Exit chat app\n");
        printf("/help             Display this help\n");
}

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

void cmd_flush(struct cmd *const cmd)
{
        cmd->cursor = 0;
        cmd->buf[0] = '\0';
}

void cmd_prompt(struct cmd *const cmd)
{
        assert(cmd != NULL);

        cmd_flush(cmd);

        printf("\n\e\[1m%s\e\[0m> %s", cmd->ps, cmd->buf);
        fflush(stdout);
}

void cmd_restore(const struct cmd *const cmd)
{
        assert(cmd != NULL);

        if (cmd->tattr) {
                fcntl(cmd->fd, F_SETFL, cmd->flags);
                tcsetattr(cmd->fd, TCSAFLUSH, cmd->tattr);
                free(cmd->tattr);
        }

        printf("\n");
}
