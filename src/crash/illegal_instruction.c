#include "crash-it/crash/illegal_instruction.h"

#include "crash-it/remote.h"
#include "crash-it/util.h"

const BYTE SHELLCODE[] = {
        0x0F, 0x0B
};

BOOL CiCrashWithIllegalInstruction(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    HANDLE thread = CiSpawnShellcode(
            process,
            SHELLCODE,
            2
    );
    if(!thread) {
        return FALSE;
    }

    CiOutputToConsole(console_out, L"Thread created, waiting for its exit...\n");
    WaitForSingleObject(thread, INFINITE);
    CiOutputToConsole(console_out, L"The remote thread exited, the process should have crashed!\n");
    CiOutputToConsole(console_out, L"If the process is still running, it might handle faults.");

    return TRUE;
}