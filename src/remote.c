#include "crash-it/remote.h"

#include "crash-it/shellcode.h"

static SIZE_T CiRemoteArgumentSize(PCI_REMOTE_ARGUMENT arg) {
    if (arg->size == 0) {
        return sizeof(arg->value);
    } else {
        return arg->size;
    }
}

static PVOID CiRemoteArgumentPtr(PCI_REMOTE_ARGUMENT arg) {
    if (arg->size == 0) {
        return &arg->value;
    } else {
        return arg->value;
    }
}

static PVOID CiRemoteArgumentCallPtr(PCI_REMOTE_CALL call, SIZE_T arg_index) {
    if(call->argument_count <= arg_index) {
        return NULL;
    }

    PCI_REMOTE_ARGUMENT arg = &call->arguments[arg_index];
    if(arg->size == 0) {
        return arg->value;
    } else {
        return arg->remote_address;
    }
}

static SIZE_T CiRemoteCallArgumentSize(PCI_REMOTE_CALL call) {
    SIZE_T out = 0;

    for (SIZE_T i = 0; i < call->argument_count; i++) {
        out += CiRemoteArgumentSize(&call->arguments[i]);
    }

    return out;
}

CI_REMOTE_ARGUMENT CiRemoteArgument(PVOID value, SIZE_T size) {
    CI_REMOTE_ARGUMENT out;
    out.value = value;
    out.size = size;

    return out;
}

CI_REMOTE_ARGUMENT CiRemoteArgumentDirect(PVOID value) {
    CI_REMOTE_ARGUMENT out;
    out.value = value;
    out.size = 0;

    return out;
}

BOOL CiPrepareRemoteCall(HANDLE process, PCI_REMOTE_CALL call) {
    SIZE_T size = CiRemoteCallArgumentSize(call);

    call->remote_allocation = VirtualAllocEx(
            process,
            NULL,
            size,
            MEM_COMMIT,
            PAGE_READWRITE
    );

    if (call->remote_allocation == NULL) {
        return FALSE;
    }

    PCHAR current_remote_write = call->remote_allocation;

    for (SIZE_T i = 0; i < call->argument_count; i++) {
        call->arguments[i].remote_address = current_remote_write;

        PVOID argument_ptr = CiRemoteArgumentPtr(&call->arguments[i]);
        SIZE_T arg_size = CiRemoteArgumentSize(&call->arguments[i]);

        WriteProcessMemory(
                process,
                current_remote_write,
                argument_ptr,
                arg_size,
                NULL
        );

        current_remote_write += arg_size;
    }

    return TRUE;
}

void CiCleanRemoteCall(HANDLE process, PCI_REMOTE_CALL call) {
    VirtualFreeEx(
            process,
            call->remote_allocation,
            0,
            MEM_RELEASE
    );
}

HANDLE CiSpawnShellcode(
        HANDLE process,
        const BYTE *shellcode,
        SIZE_T shellcode_size
) {
    PVOID remote_memory = VirtualAllocEx(
            process,
            NULL,
            shellcode_size,
            MEM_COMMIT,
            PAGE_EXECUTE_READWRITE
    );

    if (remote_memory == NULL) {
        return NULL;
    }

    if (!WriteProcessMemory(
            process,
            remote_memory,
            shellcode,
            shellcode_size,
            NULL
    )) {
        goto free_memory;
    }

    HANDLE remote_thread = CreateRemoteThread(
            process,
            NULL,
            0,
            remote_memory,
            NULL,
            0,
            NULL
    );

    if (remote_thread != NULL) {
        return remote_thread;
    }

    free_memory:
    VirtualFreeEx(process, 0, shellcode_size, MEM_RELEASE);
    return NULL;
}

HANDLE CiSpawnShellcodeCall(
        HANDLE process,
        PVOID fn,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
) {
    HANDLE heap = GetProcessHeap();

    PVOID shellcode = CiMakeCallShellcode64(
            heap,
            fn,
            arg0,
            arg1,
            arg2,
            arg3
    );

    HANDLE remote_thread = CiSpawnShellcode(
            process,
            shellcode,
            CI_CALL_SHELLCODE_SIZE_64
    );

    DWORD error = GetLastError();
    HeapFree(heap, 0, shellcode);
    SetLastError(error);

    return remote_thread;
}

HANDLE CiSpawnModuleShellcodeCall(
        HANDLE process,
        PCWSTR module,
        PCSTR function,
        PVOID arg0,
        PVOID arg1,
        PVOID arg2,
        PVOID arg3
) {
    HANDLE module_handle = GetModuleHandleW(module);
    HANDLE unload_handle = NULL;
    if(module_handle == NULL) {
        unload_handle = LoadLibraryW(module);
        if(unload_handle == NULL) {
            return NULL;
        }

        module_handle = unload_handle;
    }

    PVOID function_addr = GetProcAddress(module_handle, function);

    if(unload_handle != NULL) {
        FreeLibrary(unload_handle);
    }

    if (function_addr == NULL) {
        return NULL;
    }

    return CiSpawnShellcodeCall(
            process,
            function_addr,
            arg0,
            arg1,
            arg2,
            arg3
    );
}

HANDLE CiSpawnRemoteCall(
        HANDLE process,
        PCI_REMOTE_CALL call
) {
    PVOID arg0 = CiRemoteArgumentCallPtr(call, 0);
    PVOID arg1 = CiRemoteArgumentCallPtr(call, 1);
    PVOID arg2 = CiRemoteArgumentCallPtr(call, 2);
    PVOID arg3 = CiRemoteArgumentCallPtr(call, 3);

    switch (call->target_type) {
        case CI_REMOTE_CALL_TARGET_ADDRESS:
            return CiSpawnShellcodeCall(
                    process,
                    call->target.function,
                    arg0,
                    arg1,
                    arg2,
                    arg3
            );

        case CI_REMOTE_CALL_TARGET_KERNEL32:
            return CiSpawnModuleShellcodeCall(
                    process,
                    L"kernel32.dll",
                    call->target.export,
                    arg0,
                    arg1,
                    arg2,
                    arg3
            );

        case CI_REMOTE_CALL_TARGET_USER32:
            return CiSpawnModuleShellcodeCall(
                    process,
                    L"user32.dll",
                    call->target.export,
                    arg0,
                    arg1,
                    arg2,
                    arg3
            );
    }
}