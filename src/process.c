#include "crash-it/process.h"

#include <tlhelp32.h>
#include <shlwapi.h>

HANDLE CiOpenProcessByPid(
        DWORD process_id
) {
    return OpenProcess(
            PROCESS_CREATE_THREAD | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_SUSPEND_RESUME | PROCESS_TERMINATE |
            PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,
            FALSE,
            process_id
    );
}

HANDLE CiOpenProcessByName(
        PWSTR process_name
) {
    DWORD name_length = lstrlenW(process_name);

    PROCESSENTRY32W process_entry;
    process_entry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE toolhelp_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(toolhelp_snapshot == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    if(!Process32FirstW(toolhelp_snapshot, &process_entry)) {
        CloseHandle(toolhelp_snapshot);
        return NULL;
    }

    do {
        if(name_length != lstrlenW(process_entry.szExeFile)) {
            continue;
        }

        PCWSTR match = StrStrIW(process_entry.szExeFile, process_name);
        if(match == process_entry.szExeFile) {
            CloseHandle(toolhelp_snapshot);
            return CiOpenProcessByPid(process_entry.th32ProcessID);
        }
    } while(Process32NextW(toolhelp_snapshot, &process_entry));

    CloseHandle(toolhelp_snapshot);
    SetLastError(ERROR_NOT_FOUND);

    return NULL;
}
