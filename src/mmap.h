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

#ifndef __MMAP__H__
#define __MMAP__H__

#include "honeyprocs-common.h"
#include "status.h"

#define HP_MMAP_PAGE_SIZE 4096

typedef struct hp_mmap_tree_t hp_mmap_tree_t;

hp_status_t hp_mmap_init(hp_mmap_tree_t **mmap_tree);
hp_status_t hp_mmap_deinit(hp_mmap_tree_t *mmap_tree);
hp_status_t hp_mmap_track_memory(hp_mmap_tree_t *mmap_tree,
                                 uint32_t addr,
                                 uint32_t state,
                                 uint32_t protect,
                                 uint32_t type);
hp_status_t hp_mmap_track_memory_range(hp_mmap_tree_t *mmap_tree_dst,
                                         hp_mmap_tree_t *mmap_tree_src);
uint32_t hp_mmap_count(hp_mmap_tree_t *mmap_tree);

bool hp_are_mmaps_same(hp_mmap_tree_t *m1, hp_mmap_tree_t *m2);

void hp_mmap_print(hp_mmap_tree_t *mmap_tree);

#endif /* __MMAP__H__ */
