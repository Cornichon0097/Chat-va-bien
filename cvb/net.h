#ifndef NET_H
#define NET_H

int fetch_socket(const char *host, const char *service);

int clnt_connect(int listener);

int read_msg(int sfd, char *buf, size_t size);

#endif /* net.h */
