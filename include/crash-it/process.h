#pragma once

#include <windows.h>

HANDLE CiOpenProcessByPid(
        DWORD process_id
);

HANDLE CiOpenProcessByName(
        PWSTR process_name
);
