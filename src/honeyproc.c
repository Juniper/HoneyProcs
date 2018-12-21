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

#ifdef MIMIC_CHROME
const char *load_libs[] = {
    "advapi32.dll",
    "api-ms-win-core-synch-l1-2-0.dll",
    "api-ms-win-downlevel-advapi32-l1-1-0.dll",
    "api-ms-win-downlevel-normaliz-l1-1-0.dll",
    "api-ms-win-downlevel-ole32-l1-1-0.dll",
    "api-ms-win-downlevel-shlwapi-l1-1-0.dll",
    "api-ms-win-downlevel-user32-l1-1-0.dll",
    "api-ms-win-downlevel-version-l1-1-0.dll",
    "apisetschema.dll",
    "apphelp.dll",
    "atl.dll",
    "AudioSes.dll",
    "avrt.dll",
    "bcrypt.dll",
    "bcryptprimitives.dll",
    "cabinet.dll",
    "cfgmgr32.dll",
    "chrome_child.dll",
    "chrome.dll",
    "chrome_elf.dll",
    "clbcatq.dll",
    "comctl32.dll",
    "comdlg32.dll",
    "credssp.dll",
    "credui.dll",
    "crypt32.dll",
    "cryptbase.dll",
    "cryptnet.dll",
    "cryptsp.dll",
    "cscapi.dll",
    "cscdll.dll",
    "cscui.dll",
    "d3d11.dll",
    "D3DCompiler_47.dll",
    "dbghelp.dll",
    "dciman32.dll",
    "ddraw.dll",
    "devobj.dll",
    "devrtl.dll",
    "dhcpcsvc6.dll",
    "dhcpcsvc.dll",
    "dnsapi.dll",
    "dui70.dll",
    "duser.dll",
    "dwmapi.dll",
    "DWrite.dll",
    "dxgi.dll",
    "dxva2.dll",
    "EhStorShell.dll",
    "evr.dll",
    "ExplorerFrame.dll",
    "FirewallAPI.dll",
    "FWPUCLNT.DLL",
    "gdi32.dll",
    "gpapi.dll",
    "hid.dll",
    "IconCodecService.dll",
    "iertutil.dll",
    "imagehlp.dll",
    "imm32.dll",
    "IPHLPAPI.DLL",
    "kernel32.dll",
    "KernelBase.dll",
    "ksuser.dll",
    "libegl.dll",
    "libglesv2.dll",
    "linkinfo.dll",
    "lpk.dll",
    "mf.dll",
    "mfplat.dll",
    "MMDevAPI.dll",
    "msasn1.dll",
    "mscms.dll",
    "msctf.dll",
    "msi.dll",
    "msmpeg2vdec.dll",
    "msvcrt.dll",
    "mswsock.dll",
    "ncrypt.dll",
    "netapi32.dll",
    "netutils.dll",
    "nlaapi.dll",
    "normaliz.dll",
    "nsi.dll",
    "ntdll.dll",
    "ntmarta.dll",
    "ntshrui.dll",
    "ole32.dll",
    "oleacc.dll",
    "oleaccrc.dll",
    "oleaut32.dll",
    "powrprof.dll",
    "profapi.dll",
    "propsys.dll",
    "psapi.dll",
    "rasadhlp.dll",
    "rpcrt4.dll",
    "rsaenh.dll",
    "samcli.dll",
    "samlib.dll",
    "sechost.dll",
    "secur32.dll",
    "SensApi.dll",
    "setupapi.dll",
    "shell32.dll",
    "shlwapi.dll",
    "slc.dll",
    "srvcli.dll",
    "sspicli.dll",
    "urlmon.dll",
    "user32.dll",
    "userenv.dll",
    "usp10.dll",
    "uxtheme.dll",
    "version.dll",
    "webio.dll",
    "wevtapi.dll",
    "WindowsCodecs.dll",
    "winhttp.dll",
    "wininet.dll",
    "winmm.dll",
    "winnsi.dll",
    "winsta.dll",
    "wintrust.dll",
    "wkscli.dll",
    "wlanapi.dll",
    "wlanutil.dll",
    "Wldap32.dll",
    "Wpc.dll",
    "ws2_32.dll",
    "wship6.dll",
    "WSHTCPIP.DLL",
    "wtsapi32.dll",
};
#endif

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPtrs)
{
    /* We sleep to give the scanner time to detect the injection */
    hp_log_debug("Exception seen.  Sleep for 10s.");
    Sleep(10000);
    hp_log_debug("Woke up from sleep.  Pushing the exception forward.");

    return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
    uint32_t load_libs_count = sizeof(load_libs) / sizeof(load_libs[0]);
    uint32_t i;
    hp_status_t status;

    if (hp_log_init(HP_LOG_LEVEL_DEBUG, NULL) != HP_STATUS_OK) {
        status = HP_STATUS_OK;
        goto return_status;
    }

    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    for (i = 0; i < load_libs_count; i++) {
        if (LoadLibrary(load_libs[i]) == NULL) {
            /* \todo Only enable in debug mode */
            hp_log_debug("Failed to load library: %s", load_libs[i]);
        }
    }

    Sleep(-1);

    status = HP_STATUS_ERROR;
 return_status:
    return ((status == HP_STATUS_OK) ? 0 : -1);
}
