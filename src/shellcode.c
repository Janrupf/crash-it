#include "crash-it/shellcode.h"

#include "crash-it/util.h"

const BYTE CI_CALL_SHELLCODE_64[CI_CALL_SHELLCODE_SIZE_64] = {
/* movabs rcx, 0xDEADBEEF */ 0x48, 0xB9, 0xEF, 0xBE, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00,
/* movabs rdx, 0xDEADBEEF */ 0x48, 0xBA, 0xEF, 0xBE, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00,
/* movabs r8,  0xDEADBEEF */ 0x49, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00,
/* movabs r9,  0xDEADBEEF */ 0x49, 0xB9, 0xEF, 0xBE, 0xAD, 0xDE, 0x00, 0x00, 0x00, 0x00,
/* movabs r10, 0xDEADBEEF */ 0x49, 0xBA, 0xBE, 0xBA, 0xFE, 0xCA, 0x00, 0x00, 0x00, 0x00,
/* jmp r10                */ 0x41, 0xFF, 0xE2
};

PVOID CiMakeCallShellcode64(
        HANDLE heap,
        PVOID fn,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
) {
    char *mem = HeapAlloc(heap, 0, CI_CALL_SHELLCODE_SIZE_64);
    CiCopyMemory(mem, CI_CALL_SHELLCODE_64, CI_CALL_SHELLCODE_SIZE_64);
    CiCopyMemory(&mem[2], &arg0, 8);
    CiCopyMemory(&mem[12], &arg1, 8);
    CiCopyMemory(&mem[22], &arg2, 8);
    CiCopyMemory(&mem[32], &arg3, 8);
    CiCopyMemory(&mem[42], &fn, 8);

    return mem;
}
