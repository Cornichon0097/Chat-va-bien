#ifndef CMD_H
#define CMD_H

#include <termios.h>

#define CMD_INIT {"", NULL, NULL, 0, -1, 0}

#define CMD_BUFSIZ 2048

#define CMD_LINE_CHAR_ID '/'

#define CMD_LINE_DELIM " \t\r\n\a"

struct cmd {
        char buf[CMD_BUFSIZ];
        struct termios *tattr;
        char *ps;
        int cursor;
        int fd;
        int flags;
};

int cmd_init(struct cmd *cmd, int fd, char *ps);

void cmd_help(void);

int cmd_read(struct cmd *cmd);

void cmd_flush(struct cmd *cmd);

void cmd_prompt(struct cmd *cmd);

void cmd_restore(const struct cmd *cmd);

#endif /* cmd.h */
