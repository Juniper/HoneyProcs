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
#include "mmap.h"
#include "status.h"
#include "util-log.h"

typedef struct hp_char_buf_t {
    char buf[1024];
    uint32_t len;
} hp_char_buf_t;

static void hp_char_buf_reset(hp_char_buf_t *cbuf)
{
    memset(cbuf, 0, sizeof(*cbuf));

    return;
}

static hp_status_t hp_write_to_char_buf(hp_char_buf_t *cbuf,
                                        char *val)
{
    hp_status_t status;
    int r;

    if (cbuf->len == sizeof(cbuf->buf)) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    r = _snprintf_s(cbuf->buf + cbuf->len, sizeof(cbuf->buf) - cbuf->len,
                    _TRUNCATE, "%s", val);
    if (r <= 0) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }
    BUG_ON(r > (sizeof(cbuf->buf) - cbuf->len));
    cbuf->len += r;

    status = HP_STATUS_OK;
 return_status:
    return status;
}


static hp_status_t hp_get_process_handle(DWORD pid, LPHANDLE lph)
{
    hp_status_t status;

    *lph = OpenProcess(PROCESS_VM_READ |
                       PROCESS_VM_OPERATION |
                       PROCESS_QUERY_INFORMATION,
                       FALSE, pid);
    if (*lph == NULL) {
        hp_log_error("OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION) "
                     "failed for process with pid(%lu).  Error Code(%u).",
                     pid, GetLastError());
        status = HP_STATUS_ERROR;
        goto return_status;
    }
    hp_log_debug("OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION) "
                 "succeeded for process with pid(%lu) and got handle(0x%x).",
                 pid, *lph);

    status = HP_STATUS_OK;
 return_status:
    return status;
}

static void hp_minfo_to_string(MEMORY_BASIC_INFORMATION *minfo,
                               hp_char_buf_t *cbuf)
{
    int r;

    switch (minfo->State) {
        case MEM_COMMIT:
            hp_write_to_char_buf(cbuf, "COMMIT,");
            break;
        case MEM_FREE:
            hp_write_to_char_buf(cbuf, "FREE,");
            goto return_status;
        case MEM_RESERVE:
            hp_write_to_char_buf(cbuf, "RESERVE,");
            break;
    }

    switch (minfo->Type) {
        case MEM_IMAGE:
            hp_write_to_char_buf(cbuf, "IMAGE,");
            break;
        case MEM_MAPPED:
            hp_write_to_char_buf(cbuf, "MAPPED,");
            break;
        case MEM_PRIVATE:
            hp_write_to_char_buf(cbuf, "PRIVATE,");
            break;
        default:
            BUG_ON(1);
    }

    switch (minfo->Protect) {
        case PAGE_EXECUTE:
            hp_write_to_char_buf(cbuf, "PAGE_EXECUTE,");
            break;
        case PAGE_EXECUTE_READ:
            hp_write_to_char_buf(cbuf, "PAGE_EXECUTE_READ,");
            break;
        case PAGE_EXECUTE_READWRITE:
            hp_write_to_char_buf(cbuf, "PAGE_EXECUTE_READWRITE,");
            break;
        case PAGE_EXECUTE_WRITECOPY:
            hp_write_to_char_buf(cbuf, "PAGE_EXECUTE_WRITECOPY,");
            break;
        case PAGE_NOACCESS:
            hp_write_to_char_buf(cbuf, "PAGENOACCESS,");
            break;
        case PAGE_READONLY:
            hp_write_to_char_buf(cbuf, "PAGE_READONLY,");
            break;
        case PAGE_READWRITE:
            hp_write_to_char_buf(cbuf, "PAGE_READWRITE,");
            break;
        case PAGE_WRITECOPY:
            hp_write_to_char_buf(cbuf, "PAGE_WRITECOPY,");
            break;
#if 0
        case PAGE_TARGETS_INVALID:
            hp_write_to_char_buf(cbuf, "PAGE_TARGETS_INVALID,");
            break;
        case PAGE_TARGETS_NO_UPDATE:
            hp_write_to_char_buf(cbuf, "PAGE_TARGETS_NO_UPDATE,");
            break;
#endif
    }

 return_status:
    return;
}

hp_status_t hp_get_mmap(HANDLE ph, hp_mmap_tree_t **mmap_tree_)
{
    hp_mmap_tree_t *mmap_tree = NULL;
    MEMORY_BASIC_INFORMATION minfo;
    DWORD base_address;
    DWORD offset;
    DWORD size;
    hp_char_buf_t cbuf;
    uint32_t a;
    hp_status_t status;

    *mmap_tree_ = NULL;

    base_address = 0x00000000;
    offset = 0;
    size = 0x7FFFFFFF;
    mmap_tree = NULL;

    if (hp_mmap_init(&mmap_tree) != HP_STATUS_OK) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    while (offset < size) {
        hp_char_buf_reset(&cbuf);

        minfo.BaseAddress = (PVOID)base_address;

        /* Get information for the immediate page. */
        if (VirtualQueryEx(ph,
                           minfo.BaseAddress,
                           &minfo, sizeof(minfo)) == FALSE)
        {
            hp_log_error("VirtualQueryEx() failed to obtain permissions for "
                         "region starting at base(0x%x).  Error Code(%u).",
                         base_address, GetLastError());
            break;
        }

        hp_minfo_to_string(&minfo, &cbuf);
        //printf("%x %x - %s\n", minfo.BaseAddress, minfo.RegionSize, cbuf.buf);
        //fflush(stdout);
        if (minfo.State != MEM_FREE) {
            for (a = 0; a < minfo.RegionSize; a += HP_MMAP_PAGE_SIZE) {
                hp_mmap_track_memory(mmap_tree,
                                     (uint32_t)minfo.BaseAddress + a,
                                     minfo.State,
                                     minfo.Protect,
                                     minfo.Type);
            }
        }

        base_address += minfo.RegionSize;
        offset += minfo.RegionSize;
    }

    *mmap_tree_ = mmap_tree;

    status = HP_STATUS_OK;
 return_status:
    return status;
}

hp_status_t hp_monitor(uint32_t pid)
{
    hp_mmap_tree_t *mmap_base, *mmap_tmp;
    HANDLE ph;
    hp_status_t status;

    hp_log_debug("Monitoring pid %lu.", pid);

    if (hp_get_process_handle(pid, &ph) != HP_STATUS_OK) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    if (hp_get_mmap(ph, &mmap_base) != HP_STATUS_OK) {
        status = HP_STATUS_ERROR;
        goto return_status;
    }

    //hp_mmap_print(mmap_base);
    //exit(0);

    while (1) {
        Sleep(5000);
        hp_get_mmap(ph, &mmap_tmp);
        if (!hp_are_mmaps_same(mmap_base, mmap_tmp)) {
            //hp_log_emergency("<<<<<<<<<< INJECTION DETECTED >>>>>>>>>>>>");
            MessageBox(NULL, "INJECTION DETECTED", "HoneyProc Alert", MB_OK);
            break;
        } else {
            hp_log_debug("MMAPS SAME");
        }
        hp_mmap_deinit(mmap_tmp);
    }

    status = HP_STATUS_OK;
 return_status:
    return status;
}

void hp_print_usage()
{
    printf("scanner.exe <pid_of_honeyproc_to_monitor>\n");
}

int main(int argc, char *argv[])
{
    hp_log_init(HP_LOG_LEVEL_NONE, NULL);

    if (argc != 2) {
        hp_print_usage();
        exit(EXIT_FAILURE);
    }

    hp_monitor(atol(argv[1]));

    return 0;
}
