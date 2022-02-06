#include "crash-it/crash/hang.h"

#include <tlhelp32.h>

#include "crash-it/util.h"

BOOL CiCrashWithHang(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    DWORD process_id = GetProcessId(process);
    if(process_id == 0) {
        return FALSE;
    }

    HANDLE toolhelp_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(toolhelp_snapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    THREADENTRY32 thread_entry;
    thread_entry.dwSize = sizeof(THREADENTRY32);

    if(!Thread32First(toolhelp_snapshot, &thread_entry)) {
        CloseHandle(toolhelp_snapshot);
        return FALSE;
    }

    do {
        if(thread_entry.th32OwnerProcessID == process_id) {
            HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_entry.th32ThreadID);
            SuspendThread(thread_handle);
            CloseHandle(thread_handle);
        }
    } while(Thread32Next(toolhelp_snapshot, &thread_entry));

    CloseHandle(toolhelp_snapshot);

    return TRUE;
}
