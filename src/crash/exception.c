#include "crash-it/crash/exception.h"

#include "crash-it/util.h"
#include "crash-it/remote.h"

BOOL CiCrashWithException(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    CI_REMOTE_ARGUMENT arguments[4] = {
            CiRemoteArgumentDirect((PVOID) 0xE06D7363),
            CiRemoteArgumentDirect(0),
            CiRemoteArgumentDirect(0),
            CiRemoteArgumentDirect(NULL)
    };

    CI_REMOTE_CALL call;
    call.argument_count = 4;
    call.arguments = arguments;
    call.target_type = CI_REMOTE_CALL_TARGET_KERNEL32;
    call.target.export = "RaiseException";

    if (!CiPrepareRemoteCall(process, &call)) {
        return FALSE;
    }

    HANDLE remote_thread = CiSpawnRemoteCall(process, &call);
    CiOutputToConsole(console_out, L"Remote thread started...\n");

    WaitForSingleObject(remote_thread, INFINITE);
    CiCleanRemoteCall(process, &call);

    CiOutputToConsole(console_out, L"Remote thread terminated!\n");

    return TRUE;
}
