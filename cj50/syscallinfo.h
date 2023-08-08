#pragma once

#include <cj50/CStr.h>


static
int print_int(int n);



#define INIT_RESRET                             \
    __label__ cleanup; /* GCC extension */      \
    int ret = 0;                                \
    int res;

#define RESRET(e)                               \
    res = (e);                                  \
    if (res < 0) { ret = res; goto cleanup; }   \
    ret += res;




typedef struct SyscallInfo {
    uint8_t id;
    const char* name;
} SyscallInfo;

static
int print_debug_SyscallInfo(const SyscallInfo v) {
    INIT_RESRET;
    RESRET(print_move_cstr("(SyscallInfo){ .id = "));
    RESRET(print_int(v.id));
    RESRET(print_move_cstr(", .name = "));
    RESRET(print_debug_cstr(&v.name));
    RESRET(print_move_cstr(" }"));
cleanup:
    return ret;
}

const SyscallInfo syscallinfos[] = {
    { 0, "open" },
    { 1, "fstat" },
    { 2, "read" },
    { 3, "close" },
};

// `syscallInfoId_t` identifies a SyscallInfo instance
typedef uint16_t syscallInfoId_t;

#define SYSCALLINFO_open (syscallinfos[0])
#define SYSCALLINFO_fstat (syscallinfos[1])
#define SYSCALLINFO_read (syscallinfos[2])
#define SYSCALLINFO_close (syscallinfos[3])




#undef RESRET
#undef INIT_RESRET
