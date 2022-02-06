#include "crash-it/crash/unmap.h"

#include <ntstatus.h>
#include <tlhelp32.h>

#include "crash-it/util.h"

typedef NTSTATUS(WINAPI *PFNNTUNMAPVIEWOFSECTION)(HANDLE ProcessHandle, PVOID BaseAddress);

BOOL CiCrashWithUnmap(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
) {
    HANDLE ntdll = GetModuleHandleW(L"ntdll.dll");
    PFNNTUNMAPVIEWOFSECTION NtUnmapViewOfSection =
            (PFNNTUNMAPVIEWOFSECTION) GetProcAddress(ntdll, "NtUnmapViewOfSection");

    DWORD process_id = GetProcessId(process);
    if(process_id == 0) {
        return FALSE;
    }

    HANDLE toolhelp32_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    if(toolhelp32_snapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    MODULEENTRY32W module_entry;
    module_entry.dwSize = sizeof(MODULEENTRY32W);

    if(!Module32FirstW(toolhelp32_snapshot, &module_entry)) {
        CloseHandle(toolhelp32_snapshot);
        return FALSE;
    }

    do {
        CiOutputToConsole(console_out, L"Unmapping module ");
        CiOutputToConsole(console_out, module_entry.szExePath);

        NTSTATUS result = NtUnmapViewOfSection(process, module_entry.modBaseAddr);
        if(result != STATUS_SUCCESS) {
            CiOutputToConsole(console_out, L": FAIL\n");
        } else {
            CiOutputToConsole(console_out, L": OK\n");
        }
    } while (Module32NextW(toolhelp32_snapshot, &module_entry));

    CloseHandle(toolhelp32_snapshot);
    return TRUE;
}
