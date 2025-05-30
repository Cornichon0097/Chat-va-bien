#ifndef CVB_NET_H
#define CVB_NET_H

int fetch_socket(const char *host, const char *service);

int clnt_connect(int listener);

int send_msg(int sfd, const void *msg, size_t size);

int read_msg(int sfd, void *msg, size_t size);

#endif /* cvb/net.h */
