#pragma once

#include <windows.h>

typedef enum {
    CI_NOT_A_NUMBER,
    CI_DECIMAL_NUMBER,
    CI_HEXADECIMAL_NUMBER,
} CI_NUMBER_TYPE;

CI_NUMBER_TYPE CiIsANumber(PCWSTR input);

void CiOutputToConsole(HANDLE console, PCWSTR message);

BOOL CiReadFromConsole(HANDLE console, PWSTR buffer, DWORD buffer_size);

BOOL CiReadNumberFromConsole(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err_out,
        UINT min,
        UINT max,
        PUINT out
);

void CiWriteLastErrorAndExit(HANDLE console, PCWSTR message);

void CiExit(UINT exit_code);

void CiCopyMemory(PVOID dest, const VOID *source, SIZE_T size);
