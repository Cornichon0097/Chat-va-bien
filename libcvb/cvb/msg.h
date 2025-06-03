#ifndef CVB_MSG_H
#define CVB_MSG_H

short extract_text(int sfd, char *buf);

int write_text(int sfd, const char *buf, short size);

#endif /* cvb/msg.h */
