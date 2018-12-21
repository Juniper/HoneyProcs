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

#ifndef __AVL__H__
#define __AVL__H__

#include "honeyprocs-common.h"
#include "status.h"

typedef struct hp_avl_t hp_avl_t;
typedef void (*hp_avl_touch_user_data_func_t)(void *data, void *arg);
typedef void (*hp_avl_free_user_data_func_t)(void *data);
typedef int (*hp_avl_compare_func_t)(void *a, void *b);

hp_status_t hp_avl_init(hp_avl_t **tree,
                          hp_avl_compare_func_t compare_func,
                          hp_avl_free_user_data_func_t free_func);

hp_status_t hp_avl_deinit(hp_avl_t *tree);

hp_status_t hp_avl_add_entry(hp_avl_t *tree,
                               void *data, void **data_existing);

void *hp_avl_get(hp_avl_t *tree, void *data);

void hp_avl_parse(hp_avl_t *tree,
                   hp_avl_touch_user_data_func_t touch_func,
                   void *arg);

uint32_t hp_avl_count(hp_avl_t *tree);

#endif /* __AVL__H__ */
