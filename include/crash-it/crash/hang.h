#pragma once

#include <windows.h>

BOOL CiCrashWithHang(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err,
        HANDLE process
);
