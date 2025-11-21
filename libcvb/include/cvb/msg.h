/**
 * \file       msg.h
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
#ifndef CVB_MSG_H
#define CVB_MSG_H

#include <stdint.h>

/**
 * \brief      Maximum message buffer size.
 */
#define MSG_BUFSIZ 1024

/**
* \brief      Message codes (from client POV)
 */
#define MSG_CODE_SEND_NO_AUTH  1
#define MSG_CODE_SEND_AUTH     2
#define MSG_CODE_RECV_AUTH     3
#define MSG_CODE_SEND_PUBLIC   4
#define MSG_CODE_RECV_PUBLIC   5

/**
 * \brief      Receives a message code.
 *
 * The \c msg_recv_code() function tries to read a message code from \a sfd.
 *
 * \param[in]  sfd  The socket
 *
 * \return     The message code on success, -1 otherwise.
 */
int8_t msg_recv_code(int sfd);

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
short msg_recv_bytes(int sfd, void *buf);

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
short msg_recv_text(int sfd, char *text);

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
int msg_send_code(int sfd, int8_t code);

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
int msg_send_bytes(int sfd, const void *buf, short size);

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
int msg_send_text(int sfd, const char *text, short size);

#endif /* cvb/msg.h */
