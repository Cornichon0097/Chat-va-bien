/*
 * Functions dealing with logging events.
 * Original implementation by rxi: https://github.com/rxi
 * Modified by Antoni Blanche
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
#include <time.h>

#include "log.h"

/*
 * Logging event initializer.
 */
#define LOG_EVENT_INIT {.fmt = fmt, .file = file, .line = line, .level = level}

/*
 * Maximum callbacks.
 */
#define MAX_CALLBACKS 32

/*
 * Callback structure.
 */
struct callback {
        log_fn_t fn;
        void *udata;
        int level;
};

/*
 * The logger.
 */
static struct {
        void *udata;
        log_lock_t lock;
        int level;
        bool quiet;
        struct callback callbacks[MAX_CALLBACKS];
} logger;

/*
 * Logging levels as strings.
 */
static const char *const level_strings[] = {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

/*
 * Logging level colors.
 */
#ifdef LOG_USE_COLOR
static const char *const level_colors[] = {
        "\x1b[96m", "\x1b[92m", "\x1b[93m", "\x1b[91m", "\x1b[95m"
};
#endif

/*
 * Set time and udata of a logging event.
 */
static void set_log_event(struct log_event *ev, void *udata)
{
        time_t t = time(NULL);

        if (!ev->time)
                ev->time = localtime(&t);

        ev->udata = udata;
}

/*
 * Print a logging event.
 */
static void stdout_callback(struct log_event *ev)
{
        char buf[16];

        buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';

#ifdef LOG_USE_COLOR
        fprintf(ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
                buf, level_colors[ev->level], level_strings[ev->level],
                ev->file, ev->line);
#else
        fprintf(ev->udata, "%s %-5s %s:%d: ",
                buf, level_strings[ev->level], ev->file, ev->line);
#endif

        vfprintf(ev->udata, ev->fmt, ev->ap);
        fprintf(ev->udata, "\n");
        fflush(ev->udata);
}

/*
 * Print a logging event in a specific file.
 */
static void file_callback(struct log_event *ev)
{
        char buf[64];

        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';

        fprintf(ev->udata, "%s %-5s %s:%d: ",
                buf, level_strings[ev->level], ev->file, ev->line);

        vfprintf(ev->udata, ev->fmt, ev->ap);
        fprintf(ev->udata, "\n");
        fflush(ev->udata);
}

/*
 * Lock logging udata.
 */
static void log_lock(void)
{
        if (logger.lock)
                logger.lock(true, logger.udata);
}

/*
 * Unlock logging udata.
 */
static void log_unlock(void)
{
        if (logger.lock)
                logger.lock(false, logger.udata);
}

/*
 * Set the logging level.
 */
const char *log_level(int level)
{
        logger.level = level;

        return level_strings[level];
}

/*
 * Set the logging locker and udata.
 */
void log_locker(log_lock_t fn, void *udata)
{
        logger.lock = fn;
        logger.udata = udata;
}

/*
 * Enable quiet mod.
 */
void log_quiet(bool enable)
{
        logger.quiet = enable;
}

/*
 * Add a callback.
 */
int log_callback(log_fn_t fn, void *udata, int level)
{
        int i;

        for (i = 0; i < MAX_CALLBACKS; ++i) {
                if (!logger.callbacks[i].fn) {
                        logger.callbacks[i] = (struct callback) {
                                fn, udata, level
                        };

                        return 0;
                }
        }

        return -1;
}

/*
 * Add a callback to write in a file.
 */
int log_fp(FILE *fp, int level)
{
        return log_callback(&file_callback, fp, level);
}

/*
 * Logging function.
 */
void log_log(int level, const char *file, int line, const char *fmt, ...)
{
        struct log_event ev = LOG_EVENT_INIT;
        struct callback *cb;
        int i;

        log_lock();

        if ((!logger.quiet) && (level >= logger.level)) {
                set_log_event(&ev, stderr);
                va_start(ev.ap, fmt);
                stdout_callback(&ev);
                va_end(ev.ap);
        }

        for (i = 0; (i < MAX_CALLBACKS) && (logger.callbacks[i].fn); ++i) {
                cb = logger.callbacks + i;

                if (level >= cb->level) {
                        set_log_event(&ev, cb->udata);
                        va_start(ev.ap, fmt);
                        cb->fn(&ev);
                        va_end(ev.ap);
                }
        }

        log_unlock();
}
