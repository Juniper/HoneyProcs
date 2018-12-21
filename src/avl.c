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
#include "avl.h"
#include "status.h"

/* Max 32 bit comparison values supported and hence the height should be no
 * longer than 32 */
#define HP_AVL_MAX_HEIGHT 32

typedef struct hp_avl_node_t {
    /* node_leg[0] < node value ; node_leg[1] > node value */
    struct hp_avl_node_t *node_leg[2];
    /* Balance factor can be either +1, 0 or -1 */
    int balance;
    /* The user data */
    void *data;
} hp_avl_node_t;

typedef struct hp_avl_t {
    hp_avl_node_t *root;
    /* Compare function provided to compare node "data" values */
    hp_avl_compare_func_t compare_func;
    /* Used to free the user data during deinit*/
    hp_avl_free_user_data_func_t free_func;
    /* Total no of entries in the tree */
    uint32_t count;
} hp_avl_t;

hp_avl_node_t *hp_avl_node_alloc(void *data)
{
    hp_avl_node_t *node;
    hp_status_t status;

    if ((node = (hp_avl_node_t *)malloc(sizeof(*node))) == NULL) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }
    memset(node, 0, sizeof(*node));
    node->data = data;

    status = HP_STATUS_OK;
 return_status:
    return (status == HP_STATUS_OK) ? node : NULL;
}

void *hp_avl_get(hp_avl_t *tree, void *key)
{
    hp_avl_node_t *node;
    void *data;
    int cmp;

    data = NULL;
    node = tree->root;
    while (node != NULL) {
        cmp = tree->compare_func(key, node->data);

        if (cmp < 0) {
            node = node->node_leg[0];
        } else if (cmp > 0) {
            node = node->node_leg[1];
        } else {
            data = node->data;
            break;
        }
    }

    return data;
}

hp_status_t hp_avl_add_entry(hp_avl_t *tree,
                               void *data, void **data_existing)
{
    hp_avl_node_t *node, *node_parent;
    hp_avl_node_t *node_unbal, *node_unbal_parent;
    hp_avl_node_t *x, *y, *w, *z;
    int jmp[HP_AVL_MAX_HEIGHT];
    int jmp_idx;
    int i;
    int dir;
    int cmp;
    hp_status_t status;

    *data_existing = NULL;

    if (tree->root == NULL) {
        tree->root = hp_avl_node_alloc(data);
        if (tree->root == NULL) {
            status = HP_STATUS_ERROR;
            goto return_status;
        }
        tree->count++;
        status = HP_STATUS_OK;
        goto return_status;
    }

    node = tree->root;
    node_parent = NULL;
    /* Though the root might not be unbalanced, we still
     * need to hold it as the unbalanced point for the default
     * case where we want to adjust balances */
    node_unbal = tree->root;
    node_unbal_parent = NULL;
    jmp_idx = 0;
    while (node != NULL) {
        cmp = tree->compare_func(data, node->data);
        if (cmp == 0) {
            *data_existing = node->data;
            status = HP_STATUS_ERROR;
            goto return_status;
        }

        if (node->balance != 0) {
            jmp_idx = 0;
            node_unbal = node;
            node_unbal_parent = node_parent;
        }

        dir = cmp > 0;
        jmp[jmp_idx++] = dir;

        node_parent = node;
        node = node->node_leg[dir];
    }

    if ((node_parent->node_leg[dir] = hp_avl_node_alloc(data)) == NULL) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }
    /* Count to track the new node that has been added */
    tree->count++;

    node = node_unbal;
    i = 0;
    while (node != node_parent->node_leg[dir]) {
        if (jmp[i] == 0) {
            node->balance--;
        } else {
            node->balance++;
        }
        node = node->node_leg[jmp[i]];
        i++;
    }

    y = node_unbal;
    z = node_unbal_parent;
    if (y->balance == -2) {
        x = y->node_leg[0];
        if (x->balance == -1) {
            w = x;
            y->node_leg[0] = x->node_leg[1];
            x->node_leg[1] = y;
            if (z == NULL) {
                tree->root = w;
            } else {
                z->node_leg[z->node_leg[0] != y] = w;
            }
            x->balance = 0;
            y->balance = 0;
        } else { /* x->balance == +1 */
            BUG_ON(x->balance != +1);
            w = x->node_leg[1];
            x->node_leg[1] = w->node_leg[0];
            w->node_leg[0] = x;
            y->node_leg[0] = w->node_leg[1];
            w->node_leg[1] = y;
            if (z == NULL) {
                tree->root = w;
            } else {
                z->node_leg[z->node_leg[0] != y] = w;
            }
            if (w->balance == +1) {
                w->balance = 0;
                y->balance = 0;
                x->balance = -1;
            } else if (w->balance == -1) {
                y->balance = +1;
                x->balance = 0;
                w->balance = 0;
            } else { /* w->balance = 0 */
                x->balance = 0;
                y->balance = 0;
                w->balance = 0;
            }
        }
    } else if (y->balance == +2) {
        x = y->node_leg[1];
        if (x->balance == +1) {
            w = x;
            y->node_leg[1] = x->node_leg[0];
            x->node_leg[0] = y;
            if (z == NULL) {
                tree->root = w;
            } else {
                z->node_leg[z->node_leg[0] != y] = w;
            }
            x->balance = 0;
            y->balance = 0;
        } else { /* x->balance == -1 */
            BUG_ON(x->balance != -1);
            w = x->node_leg[0];
            x->node_leg[0] = w->node_leg[1];
            w->node_leg[1] = x;
            y->node_leg[1] = w->node_leg[0];
            w->node_leg[0] = y;
            if (z == NULL) {
                tree->root = w;
            } else {
                z->node_leg[z->node_leg[0] != y] = w;
            }
            if (w->balance == +1) {
                w->balance = 0;
                y->balance = -1;
                x->balance = 0;
            } else if (w->balance == -1) {
                y->balance = 0;
                x->balance = +1;
                w->balance = 0;
            } else { /* w->balance = 0 */
                x->balance = 0;
                y->balance = 0;
                w->balance = 0;
            }
        }
    } else {
        /* Tree balanced before insertion.  No rotation needed. */
    }

    status = HP_STATUS_OK;
 return_status:
    return status;
}

hp_status_t hp_avl_init(hp_avl_t **tree,
                          hp_avl_compare_func_t compare_func,
                          hp_avl_free_user_data_func_t free_func)
{
    hp_status_t status;

    if ((*tree = (hp_avl_t *)malloc(sizeof(**tree))) == NULL) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }
    memset(*tree, 0, sizeof(**tree));
    (*tree)->compare_func = compare_func;
    (*tree)->free_func = free_func;

    status = HP_STATUS_OK;
 return_status:
    return status;
}

static void hp_avl_parse_node(hp_avl_node_t *node,
                               hp_avl_touch_user_data_func_t touch_func,
                               void *arg)
{
    if (node == NULL)
        return;

    hp_avl_parse_node(node->node_leg[0], touch_func, arg);
    (*touch_func)(node->data, arg);
    hp_avl_parse_node(node->node_leg[1], touch_func, arg);

    return;
}

void hp_avl_parse(hp_avl_t *tree,
                   hp_avl_touch_user_data_func_t touch_func,
                   void *arg)
{
    hp_avl_parse_node(tree->root, touch_func, arg);

    return;
}

uint32_t hp_avl_count(hp_avl_t *tree)
{
    return tree->count;
}

static void hp_avl_user_data_free_stub(void *data, void *tree_)
{
    hp_avl_t *tree = (hp_avl_t *)tree_;

    tree->free_func(data);
}

hp_status_t hp_avl_deinit(hp_avl_t *tree)
{
    if (tree->free_func != NULL) {
        hp_avl_parse(tree, hp_avl_user_data_free_stub, tree);
    }
    free(tree);

    return HP_STATUS_OK;
}

