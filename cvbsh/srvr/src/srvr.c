/*
 * CVB server
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
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cvb/logger.h>
#include <cvb/msg.h>
#include <cvb/net.h>

#include "srvr.h"
#include "cvb/fdmap.h"

/*
 * Initialize server logger
 */
void srvr_set_logger(struct srvr *const srvr, const char *const pathname)
{
       srvr->log = fopen(pathname, "w");

        if (srvr->log != NULL)
                log_callback(&file_callback, srvr->log, LOG_DEBUG);
        else
                log_error("[srvr] fopen(): %s: %s", pathname, strerror(errno));

        log_debug("[srvr] Logging level set to %s", log_level(LOG_INFO));
}

/*
 * Server SIGINT handler
 */
_Noreturn static void srvr_handler(__attribute__((unused)) int signal)
{
        exit(EXIT_SUCCESS);
}

/*
 * Update server signal handler
 */
int srvr_set_handler(void)
{
        struct sigaction act;

        log_debug("[srvr] Set signal handler");

        act.sa_handler = &srvr_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        return sigaction(SIGINT, &act, NULL);
}

/*
 * Accept a new connection from a client
 */
static void srvr_connect(struct srvr *const srvr, const int sfd)
{
        int clnt;

        log_debug("[srvr] Incoming connection request");

        clnt = net_accept_clnt(sfd);

        if (clnt != -1) {
                if (fdl_add(&(srvr->fdl), clnt, POLLIN) != 0)
                        log_error("[srvr] fdl_add(): %s", strerror(errno));
                else
                        log_debug("[srvr] New client connected");
        }
}

/*
 * Send a message to all clients
 */
static void srvr_broadcast(struct srvr *const srvr, const char *const msg,
                           const char *const name)
{
        nfds_t i;

        for (i = 0; i < srvr->fdl.nfds; ++i) {
                if (srvr->fdl.fds[i].fd != srvr->listener) {
                        msg_send_code(srvr->fdl.fds[i].fd, MSG_CODE_RECV_PUBLIC);
                        msg_send_text(srvr->fdl.fds[i].fd, msg, strlen(msg));
                        msg_send_text(srvr->fdl.fds[i].fd, name, strlen(name));
                }
        }

        log_debug("[srvr] Message '%s' sent to all clients", msg);
}

/*
 * Client request processing
 */
static void srvr_recv(struct srvr *const srvr, int sfd)
{
        char buf[MSG_BUFSIZ];
        char *fdname;
        /* int clnt_fd; */
        int8_t code = msg_recv_code(sfd);

        log_debug("[srvr] Incoming client request");

        switch (code) {
        case MSG_CODE_SEND_NO_AUTH:
        case MSG_CODE_SEND_AUTH: /* TODO (requires db) */
                msg_recv_text(sfd, buf);
                log_info("[srvr] Authentification request from '%s'", buf);
                msg_send_code(sfd, MSG_CODE_RECV_AUTH);

                if (fdm_contains(&(srvr->fdm), buf) != -1) {
                        msg_send_code(sfd, 2);
                        log_debug("[srvr] Authentification failed");
                } else {
                        fdm_put(&(srvr->fdm), sfd, strdup(buf));
                        msg_send_code(sfd, 0);
                        log_debug("[srvr] Client authentified");
                }
                break;

        case MSG_CODE_SEND_PUBLIC:
                msg_recv_text(sfd, buf);
                srvr_broadcast(srvr, buf, fdm_get(&(srvr->fdm), sfd));
                break;

        /* case MSG_CODE_DM_REQUEST:
                msg_recv_text(sfd, buf);

                clnt_fd = fdm_contains(&(srvr->fdm), buf);

                if (clnt_fd == -1) {
                        msg_send_code(sfd, MSG_CODE_DM_STATUS);
                        msg_send_text(sfd, buf, strlen(buf));
                        msg_send_code(sfd, 0);
                } else {
                        fdname = fdm_get(&(srvr->fdm), sfd);

                        msg_send_code(sfd, MSG_CODE_DM_REQUEST);
                        msg_send_text(sfd, fdname, strlen(fdname));
                }
                break;

        case MSG_CODE_DM_STATUS:
                msg_recv_text(sfd, buf);

                clnt_fd = fdm_contains(&(srvr->fdm), buf);

                if (clnt_fd == -1) {
                        msg_send_code(sfd, MSG_CODE_DM_STATUS);
                        msg_send_text(sfd, buf, strlen(buf));
                        msg_send_code(sfd, 0);
                } else {
                        fdname = fdm_get(&(srvr->fdm), sfd);

                        msg_send_code(sfd, MSG_CODE_DM_REQUEST);
                        msg_send_text(sfd, fdname, strlen(fdname));
                }
                break; */

        case -1:
                fdname = fdm_remove(&(srvr->fdm), sfd);

                if (fdname != NULL)
                        log_info("[srvr] Client '%s' disconnected", fdname);
                else
                        log_info("[srvr] Client disconnected");

                fdl_remove(&(srvr->fdl), sfd);
                free(fdname);
                close(sfd);
                break;

        default:
                log_warn("[srvr] Unknown message code %hhd, ignored", code);
                break;
        }
}

/*
 * Server loop
 */
void srvr_run(struct srvr *const srvr)
{
        struct pollfd *ifd;
        int ready;

        if (fdl_add(&(srvr->fdl), srvr->listener, POLLIN) != 0) {
                log_fatal("[srvr] fdl_add(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        for (;;) {
                ready = poll(srvr->fdl.fds, srvr->fdl.nfds, -1);

                if (ready < 0) {
                        log_fatal("[srvr] poll(): %s", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                for (ifd = srvr->fdl.fds; ready > 0; ++ifd) {
                        if (ifd->revents == POLLIN) {
                                if (ifd->fd == srvr->listener)
                                        srvr_connect(srvr, ifd->fd);
                                else
                                        srvr_recv(srvr, ifd->fd);

                                --ready;
                        }
                }
        }
}

/*
 * Server destroyer
 */
void srvr_cleanup(__attribute__((unused)) int status, void *arg)
{
        struct srvr *srvr = (struct srvr *) arg;

        log_info("[srvr] Clean up and exit");

        if (srvr->log != NULL)
                fclose(srvr->log);

        /* if (srvr->dbc != NULL)
                db_close(&(srvr->dbc)); */

        if (srvr->fdl.fds != NULL)
                fdl_destroy(&(srvr->fdl));

        if (srvr->listener > -1)
                close(srvr->listener);
}
