/**
 * \file       logger.h
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
#ifndef CVB_LOGGER_H
#define CVB_LOGGER_H

#include <stdarg.h>
#include <stdbool.h>

/**
 * \brief      Logging event structure.
 */
struct log_event {
        void *udata;      /**< The log output        */
        const char *fmt;  /**< The log format        */
        va_list ap;       /**< The format subsequent */
        struct tm *time;  /**< The current time      */
        const char *file; /**< The current file      */
        int line;         /**< The current line      */
        int level;        /**< The logging level     */
};

/**
 * \brief      Logging function type.
 *
 * \see        log_callback()
 */
typedef void (*log_fn_t)(struct log_event *);

/**
 * \brief      Logging levels.
 *
 * An enumeration type of all logging levels available.
 *
 * \see        log_level()
 */
enum {LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL};

/**
 * \brief      Debug level logging function.
 *
 * Debug logging level. Use it for detailed information, typically only of
 * interest to a developer trying to diagnose a problem.
 */
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Info level logging function.
 *
 * Info logging level. Use it to confirm that things are working as expected.
 */
#define log_info(...) log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Warning level logging function.
 *
 * Warning logging level. Use it when something unexpected happened, or that a
 * problem might occur in the near future (e.g. ‘disk space low’). The software
 * is still working as expected.
 */
#define log_warn(...) log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Error level logging function.
 *
 * Error logging level. Use it when the software has not been able to perform
 * some function due to a more serious problem.
 */
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/*
 * \brief      Fatal level logging function.
 *
 * Fatal logging level. Use it for a serious error, indicating that the program
 * itself may be unable to continue running.
 */
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Enables quiet mode.
 *
 * The log_quiet() function enables or disables the logger quiet mode. When
 * enabled, no log on stdout will be performed. However, all callbacks are
 * executed.
 *
 * \param[in]  enable  Quiet mode status
 */
void log_quiet(bool enable);

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
const char *log_level(int level);

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
void log_callback(log_fn_t fn, void *udata, int level);

/**
 * \brief      Logs a message on stdout.
 *
 * The stdout_callpack() function logs a message on stdout. The main difference
 * with file_callback() function is that stdout allows to use colors in logging
 * messages.
 *
 * \param[in]  ev    The logging event
 */
void stdout_callback(struct log_event *ev);

/**
 * \brief      Logs a message in a file.
 *
 * \param[in]  ev    The logging event
 */
void file_callback(struct log_event *ev);

/**
 * \brief      Logs a message.
 *
 * \param[in]  level      The logging level
 * \param[in]  file       The current file
 * \param[in]  line       The current line
 * \param[in]  fmt        The log format
 * \param[in]  <unnamed>  The format subsequent
 */
void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif /* cvb/logger.h */
