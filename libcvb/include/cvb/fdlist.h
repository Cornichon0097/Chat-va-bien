/**
 * \file       fdlist.h
 * \brief      Functions dealing with file descriptors lists.
 *
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

/**
 * \brief      List initializer.
 */
#define FDLIST_INIT {NULL, 0, 0}

/**
 * \brief      File descriptors list.
 *
 * A structure for a list of <tt>struct pollfd<tt> with a dynamic size.
 */
struct fdlist {
        struct pollfd *fds;
        nfds_t nfds;
        nfds_t size;
};

/**
 * \brief      Adds a file descriptor.
 *
 * The \c fdl_add() function adds the file descriptor \a fd at the end of the
 * file descriptor list \a fdl.
 *
 * \param      fdl     The file descriptiors list
 * \param[in]  fd      The file descriptor to add
 * \param[in]  events  The requested events
 *
 * \return     0 on success, -1 otherwise.
 */
int fdl_add(struct fdlist *fdl, int fd, short events);

/**
 * \brief      Gets a file descriptor.
 *
 * The \c dl_get() function returns the first occurence of \a fd in the file
 * descriptors list \a fdl as a <tt>pollfd struct<tt>. If \a fd is not found in
 * \a fdl, then \c fdl_get() returns NULL.If there are other occurrences of
 * \a fd left in \a fdl, they are ignored.
 *
 * \param[in]  fdl   The file descriptors list
 * \param[in]  fd    The file descriptor wanted
 *
 * \return     The <tt>struct pollfd<tt> of \a fd.
 */
struct pollfd *fdl_get(const struct fdlist *fdl, int fd);

/**
 * \brief      Removes a file descriptor.
 *
 * The \c fdl_remove() function removes the first occurence of \a fd in the file
 * descriptors list \a fdl. After removing an element, \c fdl_remove() moves to
 * the left all the file descriptors that are after \a fd in the list. If \a fd
 * is not found in \a fdl, then \c fdl_remove() does nothing and returns -1. If
 * there are other occurrences of \a fd left in \a fdl, they are ignored.
 *
 * \param      fdl   The file descriptors list
 * \param[in]  fd    The file descriptor to remove
 *
 * \return     0 on success, -1 otherwise.
 */
int fdl_remove(struct fdlist *fdl, int fd);

/**
 * \brief      Destroys a file descriptors list.
 *
 * The \c fdl_destroy() removes all the content of the file descriptoors list
 * \a fdl and free the allocated memory.
 *
 * \param      fdl   The file descriptors list.
 */
void fdl_destroy(struct fdlist *fdl);

#endif /* cvb/fdlist.h */
