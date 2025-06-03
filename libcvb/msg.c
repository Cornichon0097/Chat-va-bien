#include <stddef.h>
#include <byteswap.h>

#include <cvb/net.h>
#include <cvb/msg.h>

#include <logger.h>

#define LITTLE_ENDIAN endian.r == 1

static union check_endian {
        unsigned int value;
        char r;
} endian = {.value = 1};

short extract_text(const int sfd, char *const buf)
{
        short text_size;

        read_msg(sfd, &text_size, sizeof(short));

        if (LITTLE_ENDIAN)
                text_size = __bswap_16(text_size);

        read_msg(sfd, buf, text_size);
        buf[text_size] = '\0';

        return text_size;
}

int write_text(const int sfd, const char *const buf, const short size)
{
        short text_size;

        if (LITTLE_ENDIAN)
                text_size = __bswap_16(size);
        else
                text_size = size;

        if (send_msg(sfd, &text_size, sizeof(short)) == -1) {
                log_error("[net] Failed to send message");
                return -1;
        }

        return send_msg(sfd, buf, text_size);
}
