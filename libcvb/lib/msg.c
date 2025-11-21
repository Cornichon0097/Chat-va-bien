/**
 * \file       msg.c
 * \brief      Functions dealing with network messages.
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

#include <arpa/inet.h>

#include <cvb/msg.h>

/**
 * \brief      Receives a message code.
 *
 * The \c msg_recv_code() function tries to read a message code from \a sfd.
 *
 * \param[in]  sfd  The socket
 *
 * \return     The message code on success, -1 otherwise.
 */
int8_t msg_recv_code(const int sfd)
{
        ssize_t nread;
        int8_t code;

        nread = recv(sfd, &code, sizeof(int8_t), 0);

        if (nread <= 0)
                return -1;

        return code;
}

/**
* \brief      Receives a message.
*
* The \c msg_recv_bytes() function tries to read bytes from \a sfd and stores it
* in \a buf.
*
* \param[in]  sfd   The socket
* \param[out] buf   The buffer
*
* \return     The number of bytes received on success, -1 otherwise.
*/
short msg_recv_bytes(int sfd, void *buf)
{
        ssize_t nread;
        short buf_size, size;

        assert(buf != NULL);

        nread = recv(sfd, &size, sizeof(short), 0);

        if (nread <= 0)
                return -1;

        buf_size = ntohs(size);

        assert(buf_size < MSG_BUFSIZ);

        nread = recv(sfd, buf, buf_size, 0);

        if (nread <= 0)
                return -1;

        return buf_size;
}

/**
 * \brief      Receives a text message.
 *
 * The \c msg_recv_text() function tries to read a text message from \a sfd and
 * stores it in \a text. A terminating null byte ('\0') is stored after the last
 * character in the buffer.
 *
 * \param[in]  sfd   The socket
 * \param[out] text  The text message
 *
 * \return     The text message size on success, -1 otherwise.
 */
short msg_recv_text(const int sfd, char *const text)
{
        short buf_size = msg_recv_bytes(sfd, text);

        if (buf_size > 0)
                text[buf_size] = '\0';

        return buf_size;
}

/**
 * \brief      Sends a message code;
 *
 * The \c msg_send_code() function tries to write the message \a code to \a sfd.
 *
 * \param[in]  sfd   The socket
 * \param[in]  code  The message code
 *
 * \return     The number of bytes sent on success, -1 otherwise.
 */
int msg_send_code(const int sfd, const int8_t code)
{
        return send(sfd, &code, sizeof(int8_t), 0);
}

/**
 * \brief      Sends a message.
 *
 * The \c msg_send_text() function tries to write \a size bytes for \a buf to
 * \a sfd.
 *
 * \param[in]  sfd   The socket
 * \param[in]  text  The buffer
 * \param[in]  size  The buffer size
 *
 * \return     The number of bytes sent on success, -1 otherwise.
 */
int msg_send_bytes(int sfd, const void *buf, short size)
{
        ssize_t nwrite;
        short buf_size = htons(size);

        assert(buf != NULL);
        assert(size < MSG_BUFSIZ);

        nwrite = send(sfd, &buf_size, sizeof(short), 0);

        if (nwrite <= 0)
                return -1;

        return send(sfd, buf, size, 0);
}

/**
 * \brief      Sends a text message.
 *
 * The \c msg_send_text() function tries to write the message \a text to \a sfd.
 *
 * \param[in]  sfd   The socket
 * \param[in]  text  The text message
 * \param[in]  size  The text message size
 *
 * \return     The number of bytes sent on success, -1 otherwise.
 */
int msg_send_text(const int sfd, const char *const text, const short size)
{
        return msg_send_bytes(sfd, text, size);
}
