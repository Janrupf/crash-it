#include "crash-it/util.h"

#include <shlwapi.h>

CI_NUMBER_TYPE CiIsANumber(PCWSTR input) {
    DWORD len = lstrlenW(input);

    BOOL maybe_hex = FALSE;

    for (DWORD i = 0; i < len; i++) {
        WCHAR c = input[i];

        if (i == 0 && c == L'0') {
            maybe_hex = TRUE;
        } else if (i == 1) {
            if (c == 'x') {
                if (!maybe_hex) {
                    return CI_NOT_A_NUMBER;
                }

                continue;
            } else {
                maybe_hex = FALSE;
            }
        }

        if (c >= L'0' && c <= L'9') {
            continue;
        } else if (c >= L'a' && c <= L'f' && maybe_hex) {
            continue;
        } else if (c >= L'A' && c <= L'F' && maybe_hex) {
            continue;
        }

        return CI_NOT_A_NUMBER;

    }

    return maybe_hex ? CI_HEXADECIMAL_NUMBER : CI_DECIMAL_NUMBER;
}

void CiOutputToConsole(HANDLE console, PCWSTR message) {
    WriteConsoleW(
            console,
            message,
            lstrlenW(message),
            NULL,
            NULL
    );
}


BOOL CiReadFromConsole(HANDLE console, PWSTR buffer, DWORD buffer_size) {
    DWORD real_size;

    if (!ReadConsoleW(
            console,
            buffer,
            buffer_size,
            &real_size,
            NULL
    )) {
        return FALSE;
    }

    buffer[real_size - 2 /* exclude \r\n */ ] = '\0';
    return TRUE;
}

BOOL CiReadNumberFromConsole(
        HANDLE console_in,
        HANDLE console_out,
        HANDLE console_err_out,
        UINT min,
        UINT max,
        PUINT out
) {
    WCHAR buffer[64];

    do {
        CiOutputToConsole(console_out, L"> ");

        if(!CiReadFromConsole(console_in, buffer, 64)) {
            return FALSE;
        }

        CI_NUMBER_TYPE type = CiIsANumber(buffer);

        if(type == CI_NOT_A_NUMBER) {
            CiOutputToConsole(console_err_out, L"Not a valid number!\n");
            continue;
        }

        StrToIntExW(
                buffer,
                type == CI_HEXADECIMAL_NUMBER ? STIF_SUPPORT_HEX : STIF_DEFAULT,
                (int *) out
        );

        if(*out < min) {
            CiOutputToConsole(console_err_out, L"Too small!\n");
            continue;
        } else if(*out > max) {
            CiOutputToConsole(console_err_out, L"Too big!\n");
            continue;
        }

        return TRUE;
    } while(1);
}

void CiWriteLastErrorAndExit(HANDLE console, PCWSTR message) {
    DWORD error = GetLastError();

    CiOutputToConsole(console, message);

    if (error == 0) {
        CiOutputToConsole(console, L"unknown error");
    } else {
        LPWSTR message_buffer = NULL;
        FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR) &message_buffer,
                0,
                NULL
        );

        CiOutputToConsole(console, message_buffer);
        LocalFree(message_buffer);
        CiExit(error);
    }
}

void CiExit(UINT exit_code) {
    DWORD proc_id;
    DWORD console_process_count = GetConsoleProcessList(&proc_id, 1);
    if (console_process_count < 2) {
        CiOutputToConsole(GetStdHandle(STD_OUTPUT_HANDLE), L"Press any key to continue...");

        HANDLE console_in = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD record;
        DWORD event_count;

        while(1) {
            if (ReadConsoleInputW(console_in, &record, 1, &event_count)) {
                if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
                    break;
                }
            } else {
                break;
            }
        }
    }

    ExitProcess(exit_code);
}

void CiCopyMemory(PVOID dest, const VOID *source, SIZE_T size) {
    PCHAR c_dest = dest;
    const CHAR *c_source = source;

    for(SIZE_T i = 0; i < size; i++) {
        c_dest[i] = c_source[i];
    }
}
