/**
 * \file       logger.c
 * \brief      Functions dealing with logging events.
 *
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
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <cvb/logger.h>


/**
 * \brief      Callback structure.
 */
struct callback {
        log_fn_t fn; /**< The callback function */
        void *udata; /**< The callback output   */
        int level;   /**< The logging level     */
};

/**
 * \brief      The logger.
 */
static struct {
        void *udata;        /**< The output        */
        int level;          /**< The logging level */
        bool quiet;         /**< The quiet mode    */
        struct callback cb; /**< The callback      */
} logger;

/**
 * \brief      Logging level strings.
 */
static const char *const level_strings[] = {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

#ifdef LOGGER_USE_COLOR
/**
 * \brief      Logging level colors.
 */
static const char *const level_colors[] = {
        "\x1b[96m", "\x1b[92m", "\x1b[93m", "\x1b[91m", "\x1b[95m"
};
#endif

/**
 * \brief      Sets a logging event.
 *
 * The set_log_event() function sets a logging event with the current time and
 * the message output.
 *
 * \param[out] ev     The logging event
 * \param[in]  udata  The logging output
 */
static void set_log_event(struct log_event *const ev, void *const udata)
{
        time_t t = time(NULL);

        if (!ev->time)
                ev->time = localtime(&t);

        ev->udata = udata;
}

/**
 * \brief      Enables quiet mode.
 *
 * The log_quiet() function enables or disables the logger quiet mode. When
 * enabled, no log on stdout will be performed. However, all callbacks are
 * executed.
 *
 * \param[in]  enable  Quiet mode status
 */
void log_quiet(const bool enable)
{
        logger.quiet = enable;
}

/**
 * \brief      Sets the logging level.
 *
 * The log_level() function sets the threshold of the logger. Logging messages
 * which are less severe than the logging level will be ignored.
 *
 * \param[in]  level  The logging level
 *
 * \return     The logging level as a string.
 */
const char *log_level(const int level)
{
        assert(level >= LOG_DEBUG);
        assert(level <= LOG_FATAL);

        logger.level = level;

        return level_strings[level];
}

/**
 * \brief      Adds a callback.
 *
 * The log_callback() function adds a callback to the logger. Each time a log is
 * performed, and if the callback severity is equal to or higher than the logger
 * threshold, the same logging message is written in the callback output.
 *
 * \param[in]  fn     The callbback logging function
 * \param[in]  udata  The callbback output
 * \param[in]  level  The callbback logging level
 */
void log_callback(log_fn_t fn, void *const udata, const int level)
{
        assert(level >= LOG_DEBUG);
        assert(level <= LOG_FATAL);

        logger.cb = (struct callback) {fn, udata, level};
}

/**
 * \brief      Logs a message on stdout.
 *
 * The stdout_callpack() function logs a message on stdout. The main difference
 * with file_callback() function is that stdout allows to use colors in logging
 * messages.
 *
 * \param[in]  ev    The logging event
 */
void stdout_callback(struct log_event *const ev)
{
        char buf[16];

        assert(ev != NULL);

        buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';

#ifdef LOGGER_USE_COLOR
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

/**
 * \brief      Logs a message in a file.
 *
 * \param[in]  ev    The logging event
 */
void file_callback(struct log_event *const ev)
{
        char buf[64];

        assert(ev != NULL);

        buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';

        fprintf(ev->udata, "%s %-5s %s:%d: ",
                buf, level_strings[ev->level], ev->file, ev->line);

        vfprintf(ev->udata, ev->fmt, ev->ap);
        fprintf(ev->udata, "\n");
        fflush(ev->udata);
}

/**
 * \brief      Logs a message.
 *
 * \param[in]  level      The logging level
 * \param[in]  file       The current file
 * \param[in]  line       The current line
 * \param[in]  fmt        The log format
 * \param[in]  <unnamed>  The format subsequent
 */
void log_log(const int level, const char *const file,
             const int line, const char *const fmt, ...)
{
        struct log_event ev = {
                .fmt = fmt, .file = file, .line = line, .level = level
        };

        if ((!logger.quiet) && (level >= logger.level)) {
                set_log_event(&ev, stderr);
                va_start(ev.ap, fmt);
                stdout_callback(&ev);
                va_end(ev.ap);
        }

        if ((logger.cb.fn != NULL) && (level >= logger.cb.level)) {
                set_log_event(&ev, logger.cb.udata);
                va_start(ev.ap, fmt);
                logger.cb.fn(&ev);
                va_end(ev.ap);
        }
}
