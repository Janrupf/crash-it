#pragma once

#include <windows.h>

typedef enum {
    CI_REMOTE_CALL_TARGET_ADDRESS,
    CI_REMOTE_CALL_TARGET_KERNEL32,
    CI_REMOTE_CALL_TARGET_USER32,
} CI_REMOTE_CALL_TARGET_TYPE;

typedef union {
    PVOID function;
    PCHAR export;
} CI_REMOTE_CALL_TARGET;

typedef struct {
    PVOID value;
    SIZE_T size;
    PVOID remote_address;
} CI_REMOTE_ARGUMENT, *PCI_REMOTE_ARGUMENT;

typedef struct {
    PCI_REMOTE_ARGUMENT arguments;
    SIZE_T argument_count;
    PVOID remote_allocation;

    CI_REMOTE_CALL_TARGET_TYPE target_type;
    CI_REMOTE_CALL_TARGET target;
} CI_REMOTE_CALL, *PCI_REMOTE_CALL;

CI_REMOTE_ARGUMENT CiRemoteArgument(PVOID value, SIZE_T size);

CI_REMOTE_ARGUMENT CiRemoteArgumentDirect(PVOID value);

BOOL CiPrepareRemoteCall(HANDLE process, PCI_REMOTE_CALL call);

void CiCleanRemoteCall(HANDLE process, PCI_REMOTE_CALL call);

HANDLE CiSpawnShellcode(
        HANDLE process,
        const BYTE *shellcode,
        SIZE_T shellcode_size
);

HANDLE CiSpawnShellcodeCall(
        HANDLE process,
        PVOID fn,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
);

HANDLE CiSpawnModuleShellcodeCall(
        HANDLE process,
        PCWSTR module,
        PCSTR function,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
);

HANDLE CiSpawnRemoteCall(
        HANDLE process,
        PCI_REMOTE_CALL call
);
