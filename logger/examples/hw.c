#include <stdlib.h>

#include "logger.h"

int main(void)
{
        log_level(LOG_DEBUG);
        log_debug("a debug message");
        log_warn("a warning message");

        log_level(LOG_WARN);
        log_debug("a debug message");
        log_warn("a warning message");

        log_quiet(true);
        log_warn("a silent message");

        log_quiet(false);
        log_error("an error message");

        return EXIT_SUCCESS;
}
