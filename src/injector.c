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

void inject(DWORD pid)
{
    HANDLE ph;
    LPVOID addr;
    uint32_t size_to_allocate = 0x1000;

    ph = OpenProcess(PROCESS_VM_READ |
                     PROCESS_VM_OPERATION |
                     PROCESS_QUERY_INFORMATION,
                     FALSE, pid);
    if (ph == NULL) {
        printf("Failure getting handle to process with pid(%u).\n", pid);
        goto return_status;
    }

    addr = VirtualAllocEx(ph, NULL, size_to_allocate, MEM_COMMIT, PAGE_EXECUTE_READ);
    if (addr == NULL) {
        printf("VirtualAllocEx() failure - %u\n", GetLastError());
        goto return_status;
    }

    printf("Allocated1 0x%x bytes at address %x in remote process with "
           "pid %u\n", size_to_allocate, addr, pid);
    fflush(stdout);

 return_status:
    return;
}

static void hp_print_usage()
{
    printf("scanner.exe <pid_of_honeyproc_to_monitor>\n");
}

int main(int argc, char *argv[])
{
    DWORD pid;

    if (argc != 2) {
        hp_print_usage();
        exit(EXIT_FAILURE);
    }

    pid = atol(argv[1]);
    inject(pid);

    return 0;
}
