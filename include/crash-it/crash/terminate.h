#pragma once

#include <windows.h>

BOOL CiCrashWithTerminate(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
);
