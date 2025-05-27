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
#ifndef CVB_FDLIST_H
#define CVB_FDLIST_H

#include <poll.h>

/*
 * File descriptors list initializer.
 */
#define FDLIST_INIT {NULL, 0, 0}

/*
 * The file descriptors list structure.
 */
struct fdlist {
        struct pollfd *fds;
        nfds_t nfds;
        nfds_t size;
};

/*
 * Add fd to fdl
 */
int fdl_add(struct fdlist *fdl, int fd, short events);

/*
 * Access the file descriptor stored at index i.
 */
struct pollfd *fdl_get(const struct fdlist *const fdl, int fd);

/*
 * Remove a file descriptor from a list.
 */
int fdl_remove(struct fdlist *fdl, int fd);

/*
 * Destroy a file descriptors list.
 */
void fdl_destroy(struct fdlist *fdl);

#endif /* cvb/fdlist.h */
