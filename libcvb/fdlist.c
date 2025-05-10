#include <stdlib.h>

#include <cvb/fdlist.h>

#define DEFAULT_SIZE 10UL

int fdl_add(struct fdlist *const fdl, const int fd)
{
        if (fdl->fds == NULL) {
                fdl->fds = (struct pollfd *)
                        malloc(DEFAULT_SIZE * sizeof(struct pollfd));

                if (fdl->fds == NULL)
                        return -1;

                fdl->size = DEFAULT_SIZE;
                fdl->cursor = 0;
        }

        if (fdl->cursor >= fdl->size) {
                fdl->fds = (struct pollfd *)
                        realloc(fdl->fds, fdl->size + DEFAULT_SIZE
                                                    * sizeof(struct pollfd));

                if (fdl->fds == NULL)
                        return -1;

                fdl->size = fdl->size + DEFAULT_SIZE;
        }

        fdl->fds[fdl->cursor].fd = fd;
        fdl->fds[fdl->cursor].events = POLLIN;
        ++fdl->cursor;

        return 0;
}

int fdl_remove(struct fdlist *const fdl, const int fd)
{
        nfds_t i, j;

        for (i = 0; i < fdl->size; ++i) {
                if (fdl->fds[i].fd == fd) {
                        for (j = i; j < fdl->size - 1; ++j)
                                fdl->fds[j] = fdl->fds[j + 1];

                        --fdl->cursor;

                        return 0;
                }
        }

        return -1;
}

void fdl_destroy(struct fdlist *const fdl)
{
        free(fdl->fds);
        fdl->fds = NULL;
        fdl->size = 0;
        fdl->cursor = 0;
}
