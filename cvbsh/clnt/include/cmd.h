#ifndef CMD_H
#define CMD_H

#define CMD_BUFSIZ 2040

#define CMD_INIT {"", 0, -1}

struct cmd {
        char buf[CMD_BUFSIZ];
        int cursor;
        int fd;
};

int cmd_init(struct cmd *cmd, int fd);

int cmd_read(struct cmd *cmd);

void cmd_prompt(const char *ps);

#endif /* cmd.h */
