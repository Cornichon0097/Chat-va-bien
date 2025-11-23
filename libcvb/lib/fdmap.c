/**
 * \file       fdmap.c
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
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <cvb/fdmap.h>

/**
 * \brief      Default file descriptors map size.
 */
#define DEFAULT_SIZE 10

/**
 * \brief      Updates a file descriptors map.
 *
 * The \c fdm_set() function updates \a fd from \a fdm with \a fdname.
 *
 * \param      fdm     The file descriptors map
 * \param[in]  fd      The file descriptor
 * \param[in]  fdname  The file descriptor name
 *
 * \return     The previous name.
 */
static char *fdm_set(struct fdmap *const fdm, const int fd,
                       char *const fdname)
{
        char *s = fdm->fdname[fd];

        fdm->fdname[fd] = fdname;

        return s;
}

/**
 * \brief      Puts a file descriptor.
 *
 * The \c fdm_put() function puts \a fdname of \a fd in \a fdm. If \a fd had
 * already a name, it is returned.
 *
 * \param      fdm     The file descriptors map
 * \param[in]  fd      The file descriptor to add
 * \param[in]  fdname  The file descriptor name
 *
 * \return     The previous name on success, -1 otherwise.
 */
char *fdm_put(struct fdmap *const fdm, int const fd, char *const fdname)
{
        assert(fdm != NULL);
        assert(fd >= 0);

        if (fdm->size <= fd) {
                fdm->fdname = (char **) realloc(fdm->fdname, fd + DEFAULT_SIZE);

                if (fdm->fdname == NULL)
                        return (char *) -1;

                memset(fdm->fdname + fdm->size, 0, fd + DEFAULT_SIZE);

                fdm->size = fd + DEFAULT_SIZE;
        }

        fdm->cursor = fd;

        return fdm_set(fdm, fd, fdname);
}

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
char *fdm_remove(struct fdmap *const fdm, const int fd)
{
        assert(fdm != NULL);
        assert(fd >= 0);
        assert(fd <= fdm->cursor);

        return fdm_set(fdm, fd, NULL);
}

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
char *fdm_get(const struct fdmap *const fdm, const int fd)
{
        assert(fdm != NULL);
        assert(fd >= 0);
        assert(fd <= fdm->cursor);

        return fdm->fdname[fd];
}

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
int fdm_contains(const struct fdmap *const fdm, const char *const fdname,
                   const size_t len)
{
        int i;

        assert(fdm != NULL);

        for (i = 0; i <= fdm->cursor; ++i) {
                if (fdm->fdname[i] != NULL) {
                        if (strncmp(fdname, fdm->fdname[i], len) == 0)
                                return i;
                }
        }

        return -1;
}

/**
 * \brief      Destroys a file descriptors map.
 *
 * The \c fdm_destroy() removes all content of \a fdm.
 *
 * \param      fdm   The file descriptors map.
 */
void fdm_destroy(struct fdmap *const fdm)
{
        assert(fdm != NULL);

        free(fdm->fdname);

        fdm->fdname = NULL;
        fdm->cursor = 0;
        fdm->size = 0;
}
