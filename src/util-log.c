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

#define _CRT_SECURE_NO_WARNINGS

#include "honeyprocs-common.h"
#include "status.h"
#include "util-log.h"

FILE * g_hp_log_fp;
hp_log_level_t g_hp_log_level;

const char *hp_log_level_to_string(hp_log_level_t log_level)
{
    switch (log_level) {
        case HP_LOG_LEVEL_EMERGENCY:
            return "Emerg";
        case HP_LOG_LEVEL_ALERT:
            return "Alert";
        case HP_LOG_LEVEL_CRITICAL:
            return "Critical";
        case HP_LOG_LEVEL_ERROR:
            return "Error";
        case HP_LOG_LEVEL_WARNING:
            return "Warning";
        case HP_LOG_LEVEL_NOTICE:
            return "Notice";
        case HP_LOG_LEVEL_INFO:
            return "Info";
        case HP_LOG_LEVEL_DEBUG:
            return "Debug";
        default:
            exit(EXIT_FAILURE);
    }
}

hp_status_t hp_log_init(hp_log_level_t log_level, const char *path)
{
    hp_status_t status;

    if (log_level == HP_LOG_LEVEL_NONE) {
        status = HP_STATUS_OK;
        goto return_status;
    }

    g_hp_log_fp = NULL;
    g_hp_log_level = log_level;

    if (path != NULL) {
        g_hp_log_fp = fopen(path, "w");
        if (g_hp_log_fp == NULL) {
            hp_log_error("Error opening file path \"%s\" for debug logging "
                          "module.", path);
            status = HP_STATUS_ERROR;
            goto return_status;
        }
    }

    status = HP_STATUS_OK;
 return_status:
    return status;
}

hp_status_t hp_log_deinit()
{
    hp_status_t status;

    if (g_hp_log_fp != NULL) {
        fclose(g_hp_log_fp);
        g_hp_log_fp = NULL;
    }

    status = HP_STATUS_OK;
    return status;
}
