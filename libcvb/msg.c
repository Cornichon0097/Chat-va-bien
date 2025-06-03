#include <byteswap.h>

#include <cvb/net.h>
#include <cvb/msg.h>

#include <logger.h>

/* #define LITTLE_ENDIAN endian.r == 1

static union check_endian {
        unsigned int value;
        char r;
} endian = {.value = 1}; */

#define LITTLE_ENDIAN false

int8_t read_code(const int sfd)
{
        int8_t code;

        if (recv_msg(sfd, &code, sizeof(int8_t)) == -1) {
                log_error("[msg] Failed to read code");
                return -1;
        }

        return code;
}

short read_text(const int sfd, char *const text)
{
        short text_size;

        if (recv_msg(sfd, &text_size, sizeof(short)) == -1) {
                log_error("[msg] Failed to read text size");
                return -1;
        }

        if (LITTLE_ENDIAN)
                text_size = __bswap_16(text_size);

        if (recv_msg(sfd, text, text_size) == -1) {
                log_error("[msg] Failed to read text");
                return -1;
        }

        text[text_size - 1] = '\0';

        return text_size;
}

int write_code(const int sfd, const int8_t code)
{
        return send_msg(sfd, &code, sizeof(int8_t));
}

int write_text(const int sfd, const char *const text, const short size)
{
        short text_size;

        if (LITTLE_ENDIAN)
                text_size = __bswap_16(size);
        else
                text_size = size;

        if (send_msg(sfd, &text_size, sizeof(short)) == -1) {
                log_error("[net] Failed to write text");
                return -1;
        }

        return send_msg(sfd, text, text_size);
}
