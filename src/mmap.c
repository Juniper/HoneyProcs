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
#include "util-log.h"
#include "mmap.h"
#include "align.h"
#include "status.h"

typedef struct hp_mmap_t {
    uint32_t page_start_addr;
    uint32_t page_end_addr;
    uint32_t state;
    uint32_t protect;
    uint32_t type;
} hp_mmap_t;

typedef struct hp_mmap_tree_t {
    hp_avl_t *mmap_tree_avl;
} hp_mmap_tree_t;

typedef struct hp_mmap_list_t {
    uint32_t size;
    uint32_t idx_insert;
    hp_mmap_t *list[];
} hp_mmap_list_t;

static hp_mmap_list_t *hp_mmap_list_alloc(uint32_t size)
{
    hp_mmap_list_t *mmap_list;
    uint32_t alloc_size;

    alloc_size = sizeof(*mmap_list) + (sizeof(hp_mmap_t *) * size);

    if ((mmap_list = malloc(alloc_size)) == NULL) {
        hp_log_error("malloc() failure.");
        goto return_status;
    }
    memset(mmap_list, 0, alloc_size);
    mmap_list->size = size;
    mmap_list->idx_insert = 0;

 return_status:
    return mmap_list;
}

static void hp_mmap_list_free(hp_mmap_list_t *mmap_list)
{
    free(mmap_list);
}

static hp_mmap_t *hp_mmap_alloc(uint32_t page_start_addr,
                                uint32_t page_end_addr,
                                uint32_t state,
                                uint32_t protect,
                                uint32_t type)
{
    hp_mmap_t *mmap;

    mmap = (hp_mmap_t *)malloc(sizeof(*mmap));
    if (mmap == NULL) {
        hp_log_error("malloc() failure.");
        goto return_status;
    }
    memset(mmap, 0, sizeof(*mmap));
    mmap->page_start_addr = page_start_addr;
    mmap->page_end_addr = page_end_addr;

 return_status:
    return mmap;
}

static void hp_mmap_free(hp_mmap_t *mmap)
{
    free(mmap);
    return;
}

static int hp_mmap_cmp(void *mmap1_, void *mmap2_)
{
    hp_mmap_t *mmap1 = (hp_mmap_t *)mmap1_;
    hp_mmap_t *mmap2 = (hp_mmap_t *)mmap2_;

    return (mmap1->page_start_addr - mmap2->page_start_addr);
}

hp_status_t hp_mmap_track_memory(hp_mmap_tree_t *mmap_tree,
                                 uint32_t addr,
                                 uint32_t state,
                                 uint32_t protect,
                                 uint32_t type)
{
    hp_mmap_t *mmap;
    hp_mmap_t *mmap_existing;
    uint32_t page_start_addr;
    uint32_t page_end_addr;
    hp_status_t status;

    page_start_addr = addr;
    ALIGN_DOWN(page_start_addr, HP_MMAP_PAGE_SIZE);
    page_end_addr = page_start_addr + (HP_MMAP_PAGE_SIZE - 1);

    mmap = hp_mmap_alloc(page_start_addr, page_end_addr,
                         state, protect, type);
    if (mmap == NULL) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    if (hp_avl_add_entry(mmap_tree->mmap_tree_avl, mmap,
                          (void **)&mmap_existing) != HP_STATUS_OK)
    {
        hp_mmap_free(mmap);

        if (mmap_existing == NULL) {
            status = HP_STATUS_ERROR;
            goto return_status;
        }
    }

    status = HP_STATUS_OK;
 return_status:
    return status;
}

static void hp_mmap_track_memory_range_(void *mmap_, void *mmap_tree_dst)
{
    hp_mmap_t *mmap = (hp_mmap_t *)mmap_;

    hp_mmap_track_memory((hp_mmap_tree_t *)mmap_tree_dst,
                         mmap->page_start_addr,
                         mmap->state,
                         mmap->protect,
                         mmap->type);

    return;
}

hp_status_t hp_mmap_track_memory_range(hp_mmap_tree_t *mmap_tree_dst,
                                         hp_mmap_tree_t *mmap_tree_src)
{
    hp_avl_parse(mmap_tree_src->mmap_tree_avl,
                  hp_mmap_track_memory_range_, mmap_tree_dst);

    return HP_STATUS_OK;
}

uint32_t hp_mmap_count(hp_mmap_tree_t *mmap_tree)
{
    return hp_avl_count(mmap_tree->mmap_tree_avl);
}

static void hp_mmap_print_page(void *mmap_, void *arg)
{
    hp_mmap_t *mmap = (hp_mmap_t *)mmap_;

    hp_log_debug("Page: %x %x", mmap->page_start_addr, mmap->page_end_addr);

    return;
}

static void hp_mmap_create_list(void *mmap_, void *mmap_list_)
{
    hp_mmap_t *mmap = (hp_mmap_t *)mmap_;
    hp_mmap_list_t *mmap_list = (hp_mmap_list_t *)mmap_list_;

    mmap_list->list[mmap_list->idx_insert++] = mmap;

    return;
}

bool hp_are_mmaps_same(hp_mmap_tree_t *mmap1_tree, hp_mmap_tree_t *mmap2_tree)
{
    uint32_t mmap1_count;
    uint32_t mmap2_count;
    hp_mmap_list_t *mmap1_list;
    hp_mmap_list_t *mmap2_list;
    hp_mmap_t *mmap1;
    hp_mmap_t *mmap2;
    uint32_t i;
    bool is_same;

    mmap1_count = hp_mmap_count(mmap1_tree);
    mmap2_count = hp_mmap_count(mmap2_tree);

    if  (mmap1_count != mmap2_count) {
        printf("mmap1_count(%u) != mmap2_count(%u)",
                     mmap1_count, mmap2_count);
        fflush(stdout);
        is_same = false;
        goto return_status;
    }

    mmap1_list = hp_mmap_list_alloc(mmap1_count);
    mmap2_list = hp_mmap_list_alloc(mmap2_count);

    hp_avl_parse(mmap1_tree->mmap_tree_avl,
                 hp_mmap_create_list, mmap1_list);
    hp_avl_parse(mmap2_tree->mmap_tree_avl,
                 hp_mmap_create_list, mmap2_list);

    for (i = 0; i < mmap1_count; i++) {
        mmap1 = mmap1_list->list[i];
        mmap2 = mmap2_list->list[i];

        if (mmap1->page_start_addr != mmap2->page_start_addr ||
            mmap1->page_end_addr != mmap2->page_end_addr ||
            mmap1->state != mmap2->state ||
            mmap1->protect != mmap2->protect ||
            mmap1->type != mmap2->type)
        {
            is_same = false;
            goto return_status;
        }
    }

    is_same = true;
 return_status:
    return is_same;
}

void hp_mmap_print(hp_mmap_tree_t *mmap_tree)
{
    hp_log_debug("Mmap:");

    hp_avl_parse(mmap_tree->mmap_tree_avl, hp_mmap_print_page, NULL);

    return;
}

hp_status_t hp_mmap_init(hp_mmap_tree_t **mmap_tree_)
{
    hp_mmap_tree_t *mmap_tree = NULL;
    hp_status_t status;

    *mmap_tree_ = NULL;

    if ((mmap_tree = (hp_mmap_tree_t *)malloc(sizeof(*mmap_tree))) == NULL) {
        hp_log_error("malloc() error allocating memory for mmap tree.");
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    if (hp_avl_init(&mmap_tree->mmap_tree_avl,
             hp_mmap_cmp,
             (hp_avl_free_user_data_func_t)hp_mmap_free) != HP_STATUS_OK)
    {
        hp_log_error("Error initializing avl tree for mmap.");
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    *mmap_tree_ = mmap_tree;

    status = HP_STATUS_OK;
 return_status:
    return status;
}

hp_status_t hp_mmap_deinit(hp_mmap_tree_t *mmap_tree)
{
    hp_avl_deinit(mmap_tree->mmap_tree_avl);
    free(mmap_tree);

    return HP_STATUS_OK;
}
