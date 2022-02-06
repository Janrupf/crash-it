#include "crash-it/crash/terminate.h"

#include "crash-it/util.h"

BOOL CiCrashWithTerminate(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    CiOutputToConsole(console_out, L"Enter exit code to use:\n");

    UINT exit_code;
    if (!CiReadNumberFromConsole(
            console_in,
            console_out,
            console_err,
            0,
            UINT_MAX,
            &exit_code
    )) {
        return FALSE;
    }

    return TerminateProcess(process, exit_code);
}