cmake_minimum_required(VERSION 3.10)
project(crash_it C ASM_MASM)

set(CMAKE_C_STANDARD 11)

set(CRASH_IT_SOURCE
        src/crash/app_exit.c
        src/crash/exception.c
        src/crash/fault.c
        src/crash/hang.c
        src/crash/illegal_instruction.c
        src/crash/terminate.c
        src/crash/unmap.c

        src/main.c
        src/process.c
        src/remote.c
        src/shellcode.c
        src/util.c

        )

macro(clear_crt_flags VAR)
    string(REGEX REPLACE "(/RTC(uc|[1ucs]|/MD.))" "" ${VAR} "${${VAR}}")
endmacro()

set(CRASH_IT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(CMAKE_MSVC_RUNTIME_LIBRARY "")

clear_crt_flags(CMAKE_C_FLAGS_DEBUG)
clear_crt_flags(CMAKE_C_FLAGS_RELEASE)

add_executable(crash-it ${CRASH_IT_SOURCE})
target_include_directories(crash-it PUBLIC ${CRASH_IT_INCLUDE_DIR})
target_compile_definitions(crash-it PUBLIC UNICODE=1 _UNICODE=1)
target_compile_options(crash-it PUBLIC /GS- /Zl)
target_link_options(crash-it PUBLIC /NODEFAULTLIB /ENTRY:main)
target_link_libraries(crash-it PUBLIC shlwapi)
