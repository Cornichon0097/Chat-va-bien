#ifndef FDLIST_H
#define FDLIST_H

#include <poll.h>

#define FDLIST_INIT {NULL, 0, 0}

#define fdl_array(fdl) (fdl.fds)
#define fdl_lenght(fdl) (fdl.cursor)
#define fdl_get(fdl, i) (fdl.fds[i])

struct fdlist {
        struct pollfd *fds;
        nfds_t size;
        nfds_t cursor;
};

int fdl_add(struct fdlist *fdl, int fd);

int fdl_remove(struct fdlist *fdl, int fd);

void fdl_destroy(struct fdlist *fdl);

#endif /* fdlist.h */
