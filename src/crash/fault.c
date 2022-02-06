#include "crash-it/crash/fault.h"

#include "crash-it/util.h"

BOOL CiCrashWithFault(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    CiOutputToConsole(console_out, L"Creating remote thread will null pointer function...\n");
    HANDLE thread = CreateRemoteThread(
            process,
            NULL,
            0,
            NULL, /* whooops */
            NULL,
            STACK_SIZE_PARAM_IS_A_RESERVATION, /* we don't really need any stack anyway */
            NULL
    );

    if(thread == NULL) {
        return FALSE;
    }

    CiOutputToConsole(console_out, L"Thread created, waiting for its exit...\n");
    WaitForSingleObject(thread, INFINITE);

    CiOutputToConsole(console_out, L"The remote thread exited, the process should have crashed!\n");
    CiOutputToConsole(console_out, L"If the process is still running, it might handle faults.");

    return TRUE;
}
