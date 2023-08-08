#pragma once

//! Wrappers around C library and POSIX functions using `Result`
//! instead of the traditional combination of in-band error signalling
//! and `errno`.

#include <cj50/syscallinfo.h> /* rename to oscallinfo ? */



#define INIT_RESRET                             \
    __label__ cleanup; /* GCC extension */      \
    int ret = 0;                                \
    int res;

#define RESRET(e)                               \
    res = (e);                                  \
    if (res < 0) { ret = res; goto cleanup; }   \
    ret += res;



/// `OsError` represents an operating system error; it is holding an
/// error nummer, which is traditionally stored in the `errno` global
/// variable.

typedef struct OsError {
    // Rust uses i32 for RawOsError, why? I only see u8 being used or
    // so. But let's not diverge.
    int32_t number; // can't use `errno` because that's a macro
} OsError;

#define OsError(errnoval) ((OsError) { .number = (errnoval) })

static UNUSED
bool equal_OsError(const OsError *a, const OsError *b) {
    return a->number == b->number;
}

// ------------------------------------------------------------------ 

/// A SystemError contains the name of the system call that failed,
/// and the system error.

typedef struct SystemError {
    syscallInfoId_t syscallinfo_id;
    OsError oserror;
} SystemError;

static
SystemError systemError(SyscallInfo syscallinfo, int _errno) {
    assert(_errno > 0);
    assert(_errno < 256);
    return (SystemError) {
        .syscallinfo_id = syscallinfo.id,
        .oserror = OsError(_errno)
    };
}

static
bool equal_SystemError(const SystemError* a, const SystemError* b) {
    return (a->syscallinfo_id == b->syscallinfo_id)
        && equal_OsError(&a->oserror, &b->oserror);
}

static
int print_debug_SystemError(const SystemError *v) {
    INIT_RESRET;
    RESRET(print_move_cstr("systemError("));
    RESRET(print_debug_SyscallInfo(syscallinfos[v->syscallinfo_id]));
    RESRET(print_move_cstr(", "));
    RESRET(print_int(v->oserror.number)); // todo: look up constant names like ENOPERM etc.?
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


// used in Result
static UNUSED
int fprintln_SystemError(FILE* out, SystemError e) {
    INIT_RESRET;
    RESRET(fprintf(out, "system error: %s: %s\n",
                   syscallinfos[e.syscallinfo_id].name,
                   strerror(e.oserror.number)));
cleanup:
    return ret;
}

// ------------------------------------------------------------------ 



#undef RESRET
#undef INIT_RESRET
