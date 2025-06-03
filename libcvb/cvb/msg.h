#ifndef CVB_MSG_H
#define CVB_MSG_H

#include <stdint.h>

#define TEXT_SIZE 1024

int8_t read_code(int sfd);

short read_text(int sfd, char *text);

int write_code(int sfd, int8_t code);

int write_text(int sfd, const char *text, short size);

#endif /* cvb/msg.h */
