/**
 * \file       fdmap.h
 * \brief      Functions dealing with file descriptors map.
 *
 * This map take benefits of the \c open() call system behavior that returns
 * the lowest-numbered file descriptor not currently open for the process.
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
#ifndef CVB_FDMAP_H
#define CVB_FDMAP_H

#include <sys/types.h>

/**
 * \brief      Map initializer.
 */
#define FDMAP_INIT {NULL, 0}

/**
 * \brief      File descriptors map.
 *
 * A structure to name file descriptors.
 */
struct fdmap {
        char **fdname;
        size_t size;
};

/**
 * \brief      Puts a file descriptor.
 *
 * The \c fdm_puts() function puts \a fdname of \a fd in \a fdm. If \a fd had
 * already a name, it is returned.
 *
 * \param      fdm     The file descriptors map
 * \param[in]  fd      The file descriptor to add
 * \param[in]  fdname  The file descriptor name
 *
 * \return     The previous name on success, -1 otherwise.
 */
char *fdm_puts(struct fdmap *fdm, int fd, char *fdname);

/**
 * \brief      Removes a file descriptor.
 *
 * The \c fdm_remove() function removes \a fd from \a fdm and return its name
 * if any.
 *
 * \param      fdm     The file descriptors map
 * \param[in]  fd      The file descriptor to remove
 *
 * \return     The file descriptor name on success.
 */
char *fdm_remove(struct fdmap *fdm, int fd);

/**
 * \brief      Returns a file descriptor name.
 *
 * The \c fdm_get() function returns the name of \a fd.
 *
 * \param[in]  fdm     The file descriptors map
 * \param[in]  fd      The file descriptor
 *
 * \return     The file descriptor name on success, NULL otherwise.
 */
char *fdm_get(const struct fdmap *fdm, int fd);

/**
 * \brief      Returns a file descriptor.
 *
 * The \c fdm_contains() function returns the file descriptor named \a fdname.
 *
 * \param[in]  fdm     The file descriptors map
 * \param[in]  fdname  The file descriptor name
 * \param[in]  len     The file descriptor name length
 *
 * \return     The file descriptor on success, -1 otherwise.
 */
int fdm_contains(const struct fdmap *fdm, const char *fdname, size_t len);

/**
 * \brief      Destroys a file descriptors map.
 *
 * The \c fdm_destroy() removes all content of \a fdm.
 *
 * \param      fdm   The file descriptors map.
 */
void fdm_destroy(struct fdmap *fdm);

#endif /* cvb/fdmap.h */
