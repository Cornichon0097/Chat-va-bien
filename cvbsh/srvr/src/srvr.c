#include <assert.h>
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

void srvr_set_logger(struct srvr *const srvr, const char *const pathname)
{
       srvr->log = fopen(pathname, "w");

        if (srvr->log != NULL)
                log_callback(&file_callback, srvr->log, LOG_DEBUG);
        else
                log_error("[srvr] fopen(): %s: %s", pathname, strerror(errno));

        log_debug("[srvr] Logging level set to %s", log_level(LOG_INFO));
}

static void srvr_handler(__attribute__((unused)) int signal)
{
        exit(EXIT_SUCCESS);
}

int srvr_set_handler(void)
{
        struct sigaction act;

        log_debug("[srvr] Set signal handler");

        act.sa_handler = &srvr_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        return sigaction(SIGINT, &act, NULL);
}

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

static void srvr_recv(struct srvr *const srvr, int sfd)
{
        char buf[MSG_BUFSIZ];
        char *fdname;
        int8_t code = msg_recv_code(sfd);

        log_debug("[srvr] Incoming client request");

        switch (code) {
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

        case MSG_CODE_SEND_NO_AUTH:
        case MSG_CODE_SEND_AUTH: /* TODO (requires db) */
                msg_recv_text(sfd, buf);
                log_info("[srvr] Authentification request from '%s'", buf);
                msg_send_code(sfd, 3);

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

        default:
                log_warn("[srvr] Unknown request %hhd, ignored", code);
                break;
        }
}

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
