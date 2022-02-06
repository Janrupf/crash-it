#include "crash-it/crash/illegal_instruction.h"

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
    LPVOID allocation = VirtualAllocEx(
            process,
            NULL,
            1024,
            MEM_COMMIT,
            PAGE_EXECUTE_READWRITE
    );

    if (allocation == NULL) {
        return FALSE;
    }

    if (!WriteProcessMemory(
            process,
            allocation,
            SHELLCODE,
            2,
            NULL
    )) {
        DWORD error = GetLastError();

        VirtualFreeEx(
                process,
                allocation,
                0,
                MEM_RELEASE
        );

        SetLastError(error);
        return FALSE;
    }

    CiOutputToConsole(console_out, L"Shellcode injected!\n");

    HANDLE thread = CreateRemoteThread(
            process,
            NULL,
            0,
            allocation,
            NULL,
            0,
            NULL
    );
    if(!thread) {
        DWORD error = GetLastError();

        VirtualFreeEx(
                process,
                allocation,
                0,
                MEM_RELEASE
        );

        SetLastError(error);
        return FALSE;
    }

    CiOutputToConsole(console_out, L"Thread created, waiting for its exit...\n");
    WaitForSingleObject(thread, INFINITE);

    VirtualFreeEx(
            process,
            allocation,
            0,
            MEM_RELEASE
    );

    CiOutputToConsole(console_out, L"The remote thread exited, the process should have crashed!\n");
    CiOutputToConsole(console_out, L"If the process is still running, it might handle faults.");

    return TRUE;
}