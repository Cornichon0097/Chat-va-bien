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
#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

/**
 * \brief      Logging event
 *
 * A structure for logging events.
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
 * \brief      Logging function type
 *
 * A type definition for logging functions for callbacks.
 *
 * \see        log_callback()
 */
typedef void (*log_fn_t)(struct log_event *);

/**
 * \brief      Lock function type
 *
 * A type definition for lock functions.
 *
 * \see        log_locker()
 */
typedef void (*log_lock_t)(bool, void *);

/**
 * \brief      Logging levels
 *
 * An enumeration type of all logging levels available.
 *
 * \see        log_level()
 */
enum {LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL};

/**
 * \brief      Debug level logging function
 *
 * Debug logging level. Use it for detailed information, typically only of
 * interest to a developer trying to diagnose a problem.
 */
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Info level logging function
 *
 * Info logging level. Use it to confirm that things are working as expected.
 */
#define log_info(...) log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Warning level logging function
 *
 * Warning logging level. Use it when something unexpected happened, or that a
 * problem might occur in the near future (e.g. ‘disk space low’). The software
 * is still working as expected.
 */
#define log_warn(...) log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Error level logging function
 *
 * Error logging level. Use it when the software has not been able to perform
 * some function due to a more serious problem.
 */
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/*
 * \brief      Fatal level logging function
 *
 * Fatal logging level. Use it for a serious error, indicating that the program
 * itself may be unable to continue running.
 */
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * \brief      Enable or disable quiet mode
 *
 * The log_quiet() function enables or disables the logger quiet mode.
 *
 * \param[in]  enable  Enable or disable quiet mode
 */
void log_quiet(bool enable);

/**
 * \brief      Set the logging level
 *
 * The log_level() function sets the threshold of the logger. Logging messages
 * which are less severe than the logging level will be ignored.
 *
 * \param[in]  level  The logging level
 *
 * \return     The logging level as a string
 */
const char *log_level(int level);

/**
 * \brief      Set a locker for the logger output
 *
 * The log_locker() function sets a locker for the logger and change the output.
 * If the lock function is NULL, no lock will be performed before logging
 * messages.
 *
 * \param[in]  fn     The lock function
 * \param[in]  udata  The logger output
 */
void log_locker(log_lock_t fn, void *udata);

/**
 * \brief      Add a callback
 *
 * The log_callback() function add a callback to the logger. Each time a log is
 * performed, and if the callback severity is equal to or higher than the logger
 * threshold, the same logging message is written in the callback output.
 *
 * \param[in]  fn     The callbback logging function
 * \param[in]  udata  The callbback output
 * \param[in]  level  The callbback logging level
 *
 * \return     0 on success, -1 otherwise
 */
int log_callback(log_fn_t fn, void *udata, int level);

/**
 * \brief      Log a message on stdout
 *
 * The stdout_callpack() function logs a message on stdout. The main difference
 * with file_callback is that stdout allows to use colors in logging messages.
 *
 * \param[in]  ev    The logging event
 */
void stdout_callback(struct log_event *ev);

/**
 * \brief      Log a message in a file
 *
 * The file_callback() function logs a message in a file.
 *
 * \param[in]  ev    The logging event
 */
void file_callback(struct log_event *ev);

/**
 * \brief      Log a message
 *
 * The log_log() function logs a message.
 *
 * \param[in]  level      The logging level
 * \param[in]  file       The current file
 * \param[in]  line       The current line
 * \param[in]  fmt        The log format
 * \param[in]  <unnamed>  The format subsequent
 */
void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif /* logger.h */
