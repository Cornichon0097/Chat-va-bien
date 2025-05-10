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
#ifndef FDLIST_H
#define FDLIST_H

#include <poll.h>

/*
 * File descriptors list initializer.
 */
#define FDLIST_INIT {NULL, 0, 0}

/*
 *
 */
struct fdlist {
        struct pollfd *fds;
        nfds_t size;
        nfds_t cursor;
};

/*
 * Return a file descriptors list as an array.
 */
#define fdl_array(fdl) (fdl.fds)

/*
 * Return the lenght of a file descriptors list.
 */
#define fdl_lenght(fdl) (fdl.cursor)

/*
 * Return a file descriptor from a list.
 */
#define fdl_get(fdl, i) (fdl.fds[i])

/*
 * Add a file descriptor in a list.
 */
int fdl_add(struct fdlist *fdl, int fd);

/*
 * Remove a file descriptor from a list.
 */
int fdl_remove(struct fdlist *fdl, int fd);

/*
 * Destroy a file descriptors list.
 */
void fdl_destroy(struct fdlist *fdl);

#endif /* fdlist.h */
