#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cvb/logger.h>
#include <cvb/msg.h>
#include <cvb/net.h>

#include "auth.h"
#include "clnt.h"
#include "cmd.h"

void clnt_set_logger(struct clnt *const clnt, const char *const pathname)
{
        clnt->log_file = fopen(pathname, "w");

        if (clnt->log_file != NULL)
                log_callback(&file_callback, clnt->log_file, LOG_DEBUG);
        else
                log_error("[clnt] fopen(): %s: %s", pathname, strerror(errno));

        log_debug("[clnt] Logging level set to %s", log_level(LOG_ERROR));
        log_debug("[clnt] Using log file %s", pathname);
}

static void clnt_handler(__attribute__((unused)) int signal)
{
        exit(EXIT_SUCCESS);
}

int clnt_set_handler(void)
{
        struct sigaction act;

        log_debug("[clnt] Set signal handler");

        act.sa_handler = &clnt_handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;

        return sigaction(SIGINT, &act, NULL);
}

int clnt_fetch_socket(char *const service)
{
        int sfd = net_fetch_socket(NULL, service);

        while (sfd == -1) {
                log_debug("[clnt] Port %s busy", service);

                sfd = net_fetch_next(NULL, service);
        }

        return sfd;
}

static void clnt_cmd(struct clnt *const clnt)
{
        if (cmd_read(&(clnt->cmd)) == '\n') {
                msg_send_code(clnt->srvr, MSG_CODE_SEND_PUBLIC);
                msg_send_text(clnt->srvr, clnt->cmd.buf, clnt->cmd.cursor);

                clnt->cmd.cursor = 0;
                clnt->cmd.buf[0] = '\0';

                cmd_prompt(&(clnt->cmd));
        }
}

static void clnt_recv(struct clnt *const clnt, int sfd)
{
        char buf[MSG_BUFSIZ];
        int8_t code = msg_recv_code(sfd);

        log_debug("[clnt] Incoming message");

        switch (code) {
        case MSG_CODE_RECV_PUBLIC:
                if (msg_recv_text(sfd, buf) == -1) {
                        log_error("[clnt] msg_recv_text(): %s", strerror(errno));
                } else {
                        printf("\r \x1b[2K");
                        printf("%s\n", buf);
                        fflush(stdout);
                        cmd_prompt(&(clnt->cmd));
                }
                break;

        case -1:
                log_fatal("[clnt] msg_recv_code(): %s", strerror(errno));
                exit(EXIT_FAILURE);
                break;

        default:
                log_warn("[clnt] Unknown message %hhd, ignored", code);
                break;
        }
}

static void clnt_connect(void)
{
        log_debug("[clnt] Inocming connection");
        log_fatal("[clnt] Function not supported");
        abort();
}

void clnt_run(struct clnt *const clnt)
{
        struct pollfd *ifd;
        int ready;

        while (auth_request(clnt->srvr, clnt->uname, MSG_BUFSIZ) != 0) {
                fprintf(stderr, "\nSorry, try again\n");
                clearerr(stdin);
        }

        if (cmd_init(&(clnt->cmd), STDIN_FILENO, clnt->uname) == -1) {
                log_fatal("[clnt] cmd_init(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        if ((fdl_add(&(clnt->fdl), STDIN_FILENO, POLLIN) != 0)
            || (fdl_add(&(clnt->fdl), clnt->listener, POLLIN) != 0)
            || (fdl_add(&(clnt->fdl), clnt->srvr, POLLIN) != 0)) {
                log_fatal("[clnt] fdl_add(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        cmd_prompt(&(clnt->cmd));

        for (;;) {
                ready = poll(clnt->fdl.fds, clnt->fdl.nfds, -1);

                if (ready < 0) {
                        log_fatal("[clnt] poll(): %s", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                for (ifd = clnt->fdl.fds; ready > 0; ++ifd) {
                        if (ifd->revents == POLLIN) {
                                if (ifd->fd == STDIN_FILENO)
                                        clnt_cmd(clnt);
                                else if (ifd->fd == clnt->listener)
                                        clnt_connect();
                                else
                                        clnt_recv(clnt, ifd->fd);

                                --ready;
                        }
                }
        }
}

void clnt_cleanup(__attribute__((unused)) int status, void *arg)
{
        struct clnt *clnt = (struct clnt *) arg;

        log_info("[clnt] Clean up and exit");

        cmd_restore(&(clnt->cmd));

        if (clnt->log_file != NULL)
                fclose(clnt->log_file);

        if (clnt->fdl.fds != NULL)
                fdl_destroy(&(clnt->fdl));

        if (clnt->srvr > -1)
                close(clnt->srvr);

        if (clnt->listener > -1)
                close(clnt->listener);
}
