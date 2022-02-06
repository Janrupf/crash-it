#include "crash-it/crash/app_exit.h"

#include "crash-it/util.h"
#include "crash-it/remote.h"

const PCWSTR TEXT = L"FatalAppExitW called by crash-it!";

BOOL CiCrashUsingFatalAppExit(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    CI_REMOTE_ARGUMENT arguments[2] = {
            CiRemoteArgumentDirect(NULL),
            CiRemoteArgument((PVOID) TEXT, (lstrlenW(TEXT) + 1) * sizeof(WCHAR)),
    };

    CI_REMOTE_CALL call;
    call.argument_count = 2;
    call.arguments = arguments;
    call.target_type = CI_REMOTE_CALL_TARGET_KERNEL32;
    call.target.export = "FatalAppExitW";

    if(!CiPrepareRemoteCall(process, &call)) {
        return FALSE;
    }

    HANDLE remote_thread = CiSpawnRemoteCall(process, &call);
    CiOutputToConsole(console_out, L"Remote thread started...\n");

    WaitForSingleObject(remote_thread, INFINITE);
    CiCleanRemoteCall(process, &call);

    CiOutputToConsole(console_out, L"Remote thread terminated!\n");

    return TRUE;
}
