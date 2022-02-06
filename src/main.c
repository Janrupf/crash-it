#include <stddef.h>
#include <windows.h>
#include <shlwapi.h>

#include "crash-it/process.h"
#include "crash-it/util.h"

#include "crash-it/crash/terminate.h"
#include "crash-it/crash/fault.h"
#include "crash-it/crash/hang.h"
#include "crash-it/crash/illegal_instruction.h"
#include "crash-it/crash/unmap.h"
#include "crash-it/crash/app_exit.h"
#include "crash-it/crash/exception.h"

int main() {
    HANDLE console_in = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE console_err = GetStdHandle(STD_ERROR_HANDLE);

    CiOutputToConsole(console_out, L"Enter name or pid of process to crash: ");

    WCHAR process_name[256];
    if (!CiReadFromConsole(console_in, process_name, 256)) {
        CiWriteLastErrorAndExit(console_err, L"Failed to read from console: ");
    }

    if (lstrlenW(process_name) < 1) {
        CiOutputToConsole(console_err, L"Invalid process name!\n");
        CiExit(STATUS_INVALID_PARAMETER);
    }

    HANDLE target_process;

    CI_NUMBER_TYPE type = CiIsANumber(process_name);
    if (type != CI_NOT_A_NUMBER) {
        CiOutputToConsole(console_out, L"Interpreting process name as a PID...\n");

        DWORD pid;
        StrToIntExW(
                process_name,
                type == CI_HEXADECIMAL_NUMBER ? STIF_SUPPORT_HEX : STIF_DEFAULT,
                (int *) &pid
        );

        target_process = CiOpenProcessByPid(pid);
        if (target_process == NULL) {
            CiWriteLastErrorAndExit(console_err, L"Failed to open process by PID: ");
        }
    } else {
        target_process = CiOpenProcessByName(process_name);
        if (target_process == NULL) {
            CiWriteLastErrorAndExit(console_err, L"Failed to open process by name: ");
        }
    }

    CiOutputToConsole(console_out, L"--- Choose a crash method\n");
    CiOutputToConsole(console_out, L" [0] TerminateProcess\n");
    CiOutputToConsole(console_out, L" [1] CreateRemoteThread   - bad function pointer\n");
    CiOutputToConsole(console_out, L" [2] CreateRemoteThread   - illegal instruction\n");
    CiOutputToConsole(console_out, L" [3] SuspendThread        - suspend all threads\n");
    CiOutputToConsole(console_out, L" [4] NtUnmapViewOfSection - unmap process from memory\n");
    CiOutputToConsole(console_out, L" [5] AppFatalExit\n");
    CiOutputToConsole(console_out, L" [6] RaiseException       - SEH C++ Uncaught exception\n");
    CiOutputToConsole(console_out, L"---\n");

    UINT choice;
    if (!CiReadNumberFromConsole(
            console_in,
            console_out,
            console_err,
            0,
            6,
            &choice
    )) {
        CiWriteLastErrorAndExit(console_err, L"Failed to read number: ");
    }

    BOOL success;
    switch (choice) {
        case 0:
            success = CiCrashWithTerminate(console_in, console_out, console_err, target_process);
            break;

        case 1:
            success = CiCrashWithFault(console_in, console_out, console_err, target_process);
            break;

        case 2:
            success = CiCrashWithIllegalInstruction(console_in, console_out, console_err, target_process);
            break;

        case 3:
            success = CiCrashWithHang(console_in, console_out, console_err, target_process);
            break;

        case 4:
            success = CiCrashWithUnmap(console_in, console_out, console_err, target_process);
            break;

        case 5:
            success = CiCrashUsingFatalAppExit(console_in, console_out, console_err, target_process);
            break;

        case 6:
            success = CiCrashWithException(console_in, console_out, console_err, target_process);
            break;
    }

    CloseHandle(target_process);

    if(!success) {
        CiWriteLastErrorAndExit(console_err, L"Failed to crash process: ");
    }

    return 0;
}