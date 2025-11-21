#ifndef AUTH_H
#define AUTH_H

#include <sys/types.h>

int auth_request(int srvr, char *const uname, size_t size);

#endif /* auth.h */
