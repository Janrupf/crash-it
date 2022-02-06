#pragma once

#include <windows.h>

#define CI_CALL_SHELLCODE_SIZE_64 53

PVOID CiMakeCallShellcode64(
        HANDLE heap,
        PVOID fn,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
);
