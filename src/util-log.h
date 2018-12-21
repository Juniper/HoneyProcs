/**
 * Copyright(C) 2018, Juniper Networks, Inc.
 * All rights reserved
 *
 * This SOFTWARE is licensed under the license provided in the LICENSE.txt
 * file.  By downloading, installing, copying, or otherwise using the
 * SOFTWARE, you agree to be bound by the terms of that license.  This
 * SOFTWARE is not an official Juniper product.
 *
 * Third-Party Code: This SOFTWARE may depend on other components under
 * separate copyright notice and license terms.  Your use of the source
 * code for those components is subject to the term and conditions of
 * the respective license as noted in the Third-Party source code.
 */

/**
 * @author Anoop Saldanha
 */

/* Logging utility */

#ifndef __UTIL_LOG__H__
#define __UTIL_LOG__H__

#include "honeyprocs-common.h"
#include "status.h"

#ifdef WIN32
#define __FILENAME__ (strrchr(__FILE__, '\\') ? \
                      strrchr(__FILE__, '\\') + 1 : \
                      __FILE__)
#endif

/**
 * The log levels.  One needs to specify the log level of the alerts needed
 * in their call to EyLogInit().
 */
typedef enum hp_log_level_t {
    HP_LOG_LEVEL_NOTSET = -1,
    HP_LOG_LEVEL_NONE = 0,
    HP_LOG_LEVEL_EMERGENCY,
    HP_LOG_LEVEL_ALERT,
    HP_LOG_LEVEL_CRITICAL,
    HP_LOG_LEVEL_ERROR,
    HP_LOG_LEVEL_WARNING,
    HP_LOG_LEVEL_NOTICE,
    HP_LOG_LEVEL_INFO,
    HP_LOG_LEVEL_DEBUG,
    HP_LOG_LEVEL_MAX,
} hp_log_level_t;

extern FILE *g_hp_log_fp;
extern hp_log_level_t g_hp_log_level;

#define hp_log(log_level, ...)                                          \
    do {                                                                \
        if (log_level <= g_hp_log_level) {                              \
            char buf1[1024];                                            \
            char buf2[1024];                                            \
            _snprintf_s(buf1, sizeof(buf1), _TRUNCATE, __VA_ARGS__);    \
            _snprintf_s(buf2, sizeof(buf2), _TRUNCATE,                  \
                        "(%s:%d)(%s) <%s> - %s\r\n",                    \
                        __FILENAME__, __LINE__,                         \
                        __FUNCTION__,                                   \
                        hp_log_level_to_string(log_level), buf1);       \
            fprintf(stdout, "%s", buf2);                                \
            fflush(stdout);                                             \
            if (g_hp_log_fp != NULL) {                                  \
                fprintf(g_hp_log_fp, "%s", buf2, strlen(buf2));         \
                fflush(g_hp_log_fp);                                   \
            }                                                           \
        }                                                               \
    } while (0)

/**
 * Private API.
 */
const char *hp_log_level_to_string(hp_log_level_t log_level);

/**
 * Various logging APIs.
 */
#define hp_log_emergency(...) hp_log(HP_LOG_LEVEL_EMERGENCY, __VA_ARGS__)
#define hp_log_alert(...) hp_log(HP_LOG_LEVEL_ALERT, __VA_ARGS__)
#define hp_log_critical(...) hp_log(HP_LOG_LEVEL_CRITICAL, __VA_ARGS__)
#define hp_log_error(...) hp_log(HP_LOG_LEVEL_ERROR, __VA_ARGS__)
#define hp_log_warning(...) hp_log(HP_LOG_LEVEL_WARNING, __VA_ARGS__)
#define hp_log_notice(...) hp_log(HP_LOG_LEVEL_NOTICE, __VA_ARGS__)
#define hp_log_info(...) hp_log(HP_LOG_LEVEL_INFO, __VA_ARGS__)
#define hp_log_debug(...) hp_log(HP_LOG_LEVEL_DEBUG, __VA_ARGS__)

/**
 * Initialize the logging API.
 *
 * @log_level The log level to be used as offered by EyLogLevel_t.
 * @path Path to a log file to write to.  It can also be NULL in which
 *       case logging to a file is disabled and only console logging is used.
 *
 * @retval HP_STATUS_OK On successful init.
 * @retval HP_STATUS_ERROR On an un-successful init.
 */
hp_status_t hp_log_init(hp_log_level_t log_level, const char *path);

/**
 * DeInitialize the logging API.
 *
 * @retval HP_STATUS_OK On successful de-init.
 * @retval HP_STATUS_ERROR On an un-successful de-init.
 */
hp_status_t hp_log_deinit();

#endif /* __UTIL_LOG__H__ */
