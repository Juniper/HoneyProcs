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

#include "honeyprocs-common.h"
#include "util-log.h"
#include "status.h"

int hp_scan_engine_run(uint8_t *buf, uint32_t buf_len)
{
    uint32_t i = 0;
    uint8_t *pat = (uint8_t *)"hahalala";
    uint32_t patlen = strlen((char *)pat);
    uint32_t max = buf_len - patlen;

    for (i = 0; i < max; i++) {
        if (memcmp(buf + i, pat, patlen) == 0) {
            return i;
        }
    }

    return 0;
}
