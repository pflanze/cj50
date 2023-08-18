#pragma once

#include <cj50/CStr.h>
#include "cj50/int.h"
#include "cj50/resret.h"


typedef struct SyscallInfo {
    uint8_t id;
    int8_t manpage_section; // 2 => POSIX, 3=> C library
    const char* name;
} SyscallInfo;

static
int print_debug_SyscallInfo(const SyscallInfo v) {
    INIT_RESRET;
    RESRET(print_move_cstr("(SyscallInfo){ .id = "));
    RESRET(print_move_int(v.id));
    RESRET(print_move_cstr(", .manpage_section = "));
    RESRET(print_move_int(v.manpage_section));
    RESRET(print_move_cstr(", .name = "));
    RESRET(print_debug_cstr(&v.name));
    RESRET(print_move_cstr(" }"));
cleanup:
    return ret;
}

const SyscallInfo syscallinfos[] = {
    { 0, 2, "open" },
    { 1, 2, "fstat" },
    { 2, 2, "read" },
    { 3, 2, "close" },
    { 4, 3, "getc" },
    { 5, 3, "getc_unlocked" },
    { 6, 3, "fclose" },
    { 7, 3, "fopen" },
    { 8, 3, "fflush" },
    { 9, 2, "fsync" },
    { 10, 2, "fdatasync" },
    { 11, 3, "fmemopen" },
    { 12, 3, "pthread_create" }, // POSIX threads but it's in section 3 ??
    { 13, 3, "pthread_join" },
};

// `syscallInfoId_t` identifies a SyscallInfo instance
typedef uint16_t syscallInfoId_t;

#define SYSCALLINFO_open (syscallinfos[0])
#define SYSCALLINFO_fstat (syscallinfos[1])
#define SYSCALLINFO_read (syscallinfos[2])
#define SYSCALLINFO_close (syscallinfos[3])
#define SYSCALLINFO_getc (syscallinfos[4])
#define SYSCALLINFO_getc_unlocked (syscallinfos[5])
#define SYSCALLINFO_fclose (syscallinfos[6])
#define SYSCALLINFO_fopen (syscallinfos[7])
#define SYSCALLINFO_fflush (syscallinfos[8])
#define SYSCALLINFO_fsync (syscallinfos[9])
#define SYSCALLINFO_fdatasync (syscallinfos[10])
#define SYSCALLINFO_fmemopen (syscallinfos[11])
#define SYSCALLINFO_pthread_create (syscallinfos[12])
#define SYSCALLINFO_pthread_join (syscallinfos[13])

