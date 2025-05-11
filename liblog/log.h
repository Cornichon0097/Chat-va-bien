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
#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

/*
 * Logging event structure.
 */
struct log_event {
        void *udata;
        const char *fmt;
        va_list ap;
        struct tm *time;
        const char *file;
        int line;
        int level;
};

/*
 * Logging function type.
 */
typedef void (*log_fn_t)(struct log_event *);

/*
 * Logging lock type.
 */
typedef void (*log_lock_t)(bool, void *);

/*
 * Logging levels.
 */
enum { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

/*
 * Debug logging level. Use it for detailed information, typically only of
 * interest to a developer trying to diagnose a problem.
 */
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Info logging level. Use it to confirm that things are working as expected.
 */
#define log_info(...) log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/*
 * Warning logging level. Use it when something unexpected happened, or that a
 * problem might occur in the near future (e.g. ‘disk space low’). The software
 * is still working as expected.
 */
#define log_warn(...) log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/*
 * Error logging level. Use it when the software has not been able to perform
 * some function due to a more serious problem.
 */
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Fatal logging level. Use it for a serious error, indicating that the program
 * itself may be unable to continue running.
 */
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/*
 * Set the logging level.
 */
const char *log_level(int level);

/*
 * Set the logging locker and udata.
 */
void log_locker(log_lock_t fn, void *udata);

/*
 * Enable quiet mod.
 */
void log_quiet(bool enable);

/*
 * Add a callback.
 */
int log_callback(log_fn_t fn, void *udata, int level);

/*
 * Add a callback to write in a file.
 */
int log_fp(FILE *fp, int level);

/*
 * Logging function.
 */
void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif /* log.h */
