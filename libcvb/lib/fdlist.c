/**
 * \file       fdlist.c
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
#include <assert.h>
#include <stdlib.h>

#include <cvb/fdlist.h>

/**
 * \brief      Default file descriptors list size.
 */
#define DEFAULT_SIZE 16

/**
 * \brief      Performs a left shift.
 *
 * The \c _fdl_shift_left() function moves to the left each elements of the
 * file descriptors list \a fdl from \a start.
 *
 * \param      fdl    The file descriptors list
 * \param[in]  start  The first element to move
 */
/* static void _fdl_shift_left(struct fdlist *const fdl, const nfds_t start)
{
        nfds_t i;

        for (i = start; i < fdl->nfds - 1; ++i)
                fdl->fds[i] = fdl->fds[i + 1];
} */

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
int fdl_add(struct fdlist *const fdl, const int fd, const short events)
{
        nfds_t i;

        assert(fdl != NULL);

        for (i = 0; i < fdl->nfds; ++i) {
                if (fdl->fds[i].fd == -1) {
                        fdl->fds[i].fd = fd;
                        fdl->fds[i].events = events;

                        return 0;
                }
        }

        if (i >= fdl->size) {
                fdl->size = fdl->size + DEFAULT_SIZE;
                fdl->fds = (struct pollfd *) realloc(fdl->fds, (fdl->size)
                                                     * sizeof(struct pollfd));

                if (fdl->fds == NULL)
                        return -1;
        }

        fdl->fds[fdl->nfds].fd = fd;
        fdl->fds[fdl->nfds].events = events;
        ++fdl->nfds;

        return 0;
}

/**
 * \brief      Gets a file descriptor.
 *
 * The \c dl_get() function returns the first occurence of \a fd in the file
 * descriptors list \a fdl as a <tt>struct pollfd<tt>. If \a fd is not found in
 * \a fdl, or if \a fd is negative, then \c fdl_get() returns NULL.If there are
 * other occurrences of \a fd left in \a fdl, they are ignored.
 *
 * \param[in]  fdl   The file descriptors list
 * \param[in]  fd    The file descriptor wanted
 *
 * \return     The <tt>struct pollfd<tt> of \a fd.
 */
struct pollfd *fdl_get(const struct fdlist *const fdl, const int fd)
{
        nfds_t i;

        assert(fdl != NULL);

        if (fd < 0)
                return NULL;

        for (i = 0; i < fdl->nfds; ++i) {
                if (fdl->fds[i].fd == fd)
                        return fdl->fds + i;
        }

        return NULL;
}

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
int fdl_remove(struct fdlist *const fdl, const int fd)
{
        nfds_t i;

        assert(fdl != NULL);

        if (fd < 0)
                return -1;

        for (i = 0; i < fdl->nfds; ++i) {
                if (fdl->fds[i].fd == fd) {
                        fdl->fds[i].fd = -1;

                        return 0;
                }
        }

        return -1;
}

/**
 * \brief      Destroys a file descriptors list.
 *
 * The \c fdl_destroy() removes all the content of the file descriptoors list
 * \a fdl and free the allocated memory.
 *
 * \param      fdl   The file descriptors list.
 */
void fdl_destroy(struct fdlist *const fdl)
{
        assert(fdl != NULL);

        free(fdl->fds);

        fdl->fds = NULL;
        fdl->nfds = 0;
        fdl->size = 0;
}
