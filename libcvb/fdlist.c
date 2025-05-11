/*
 * Functions dealing with file descriptors lists.
 * Copyright (c) 2025 Antoni Blanche
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>

#include <cvb/fdlist.h>

/*
 * Default file descriptors list size.
 */
#define DEFAULT_SIZE 10UL

/*
 * Perform a left shift on a file descriptors list.
 */
static void fdl_shift_left(struct fdlist *const fdl, const nfds_t start)
{
        nfds_t i;

        for (i = start; i < fdl->size - 1; ++i)
                fdl->fds[i] = fdl->fds[i + 1];
}

/*
 * Add a file descriptor in a list.
 */
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

/*
 * Remove a file descriptor from a list.
 */
int fdl_remove(struct fdlist *const fdl, const int fd)
{
        nfds_t i;

        for (i = 0; i < fdl->size; ++i) {
                if (fdl->fds[i].fd == fd) {
                        fdl_shift_left(fdl, i);
                        --fdl->cursor;
                        return 0;
                }
        }

        return -1;
}

/*
 * Destroy a file descriptors list.
 */
void fdl_destroy(struct fdlist *const fdl)
{
        free(fdl->fds);
        fdl->fds = NULL;
        fdl->size = 0;
        fdl->cursor = 0;
}
