#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <cvb/net.h>
#include <cvb/msg.h>
#include <cvb/fdlist.h>

#include <logger.h>

#include <srvr.h>
#include <db.h>

#define REQUEST_NO_AUTH_CONNECT 1
#define REQUEST_AUTH_CONNECT 2

/*
 * Infinite timeout for the poll() function.
 */
#define NO_TIMEOUT -1

void auth_request(__attribute__((unused)) struct srvr *const srvr, const int sfd, __attribute__((unused)) const char flag)
{
        char buf[TEXT_SIZE];
        short nread;

        nread = read_text(sfd, buf);
        printf("%s\n", buf);
        write_code(sfd, 0);
        write_text(sfd, buf, nread);
}

void srvr_recv(struct srvr *const srvr, int sfd)
{
        int8_t code;
        int clnt;

        if (sfd == srvr->listener) {
                log_debug("[srvr] Received connection request");

                clnt = clnt_accept(sfd);

                if (clnt != -1)
                        fdl_add(&(srvr->fdl), clnt, POLLIN);
        } else {
                if (recv_msg(sfd, &code, sizeof(int8_t)) > 0) {
                        switch (code) {
                        case REQUEST_NO_AUTH_CONNECT:
                        case REQUEST_AUTH_CONNECT:
                                log_debug("[srvr] Received auth request");
                                auth_request(srvr, sfd, code);
                                break;
                        default:
                                log_error("[srvr] Unrecognized request %c", code);
                                break;
                        }
                } else {
                        log_info("[srvr] Client disconnected");
                        fdl_remove(&(srvr->fdl), sfd);
                        close(sfd);
                }
        }

}

void loop(struct srvr *const srvr)
{
        struct pollfd *ifd;
        int ready;

        if (fdl_add(&(srvr->fdl), srvr->listener, POLLIN) != 0) {
                log_fatal("[srvr] fdl_add(): %s", strerror(errno));
                exit(EXIT_FAILURE);
        }

        for (;;) {
                ready = poll(srvr->fdl.fds, srvr->fdl.nfds, NO_TIMEOUT);

                if (ready < 0) {
                        log_fatal("[srvr] poll(): %s", strerror(errno));
                        exit(EXIT_FAILURE);
                }

                for (ifd = srvr->fdl.fds; ready > 0; ++ifd) {
                        if (ifd->revents == POLLIN) {
                                srvr_recv(srvr, ifd->fd);
                                --ready;
                        }
                }
        }
}
