#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cj50/CStr.h>
#include <cj50/gen/Result.h>

static UNUSED
char* new_cstr(size_t len);

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


// ------------------------------------------------------------------ 


typedef struct SyscallInfo {
    uint8_t id;
    const char* name;
} SyscallInfo;

static
int print_debug_SyscallInfo(const SyscallInfo v) {
    INIT_RESRET;
    RESRET(print_cstr("(SyscallInfo){ .id = "));
    RESRET(print_int(v.id));
    RESRET(print_cstr(", .name = "));
    RESRET(print_debug_cstr(&v.name));
    RESRET(print_cstr(" }"));
cleanup:
    return ret;
}

const SyscallInfo syscallinfos[] = {
    { 0, "open" },
    { 1, "fstat" },
    { 2, "read" },
    { 3, "close" },
};

#define SYSCALLINFO_open (syscallinfos[0])
#define SYSCALLINFO_fstat (syscallinfos[1])
#define SYSCALLINFO_read (syscallinfos[2])
#define SYSCALLINFO_close (syscallinfos[3])


/// A SystemError contains the name of the system call that failed,
/// and the system error.

typedef struct SystemError {
    uint8_t syscallinfo_id;
    uint8_t _errno;
} SystemError;
// `errno` is a macro!, bummer

static
SystemError systemError(SyscallInfo syscallinfo, int _errno) {
    assert(errno > 0);
    assert(errno < 256);
    return (SystemError) { .syscallinfo_id = syscallinfo.id, ._errno = _errno };
}

static
bool equal_SystemError(const SystemError* a, const SystemError* b) {
    return (a->syscallinfo_id == b->syscallinfo_id)
        && (a->_errno == b->_errno);
}

static
int print_debug_SystemError(const SystemError *v) {
    INIT_RESRET;
    RESRET(print_cstr("systemError("));
    RESRET(print_debug_SyscallInfo(syscallinfos[v->syscallinfo_id]));
    RESRET(print_cstr(", "));
    RESRET(print_int(v->_errno)); // todo: look up constant names like ENOPERM etc.?
    RESRET(print_cstr(")"));
cleanup:
    return ret;
}


// used in Result
static UNUSED
int fprintln_SystemError(FILE* out, SystemError e) {
    INIT_RESRET;
    RESRET(fprintf(out, "system error: %s: %s\n",
                   syscallinfos[e.syscallinfo_id].name,
                   strerror(e._errno)));
cleanup:
    return ret;
}




GENERATE_Result(String, SystemError);

// todo: use a cstr with len so that \0 will not be lost?

/// Returns a copy of the contents of the file at the given `path` as
/// a cstr, if possible (no system errors occurred).

/// Note: the String has a '\0' terminator character added. Any '\0'
/// characters contained in the file will be part of the returned
/// String and lead to it being interpreted as terminating there.

Result(String, SystemError) filecontents_String(cstr path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        int e = errno;
        return Err(String, SystemError)(systemError(SYSCALLINFO_open, e));
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        int e = errno;
        close(fd);
        return Err(String, SystemError)(systemError(SYSCALLINFO_fstat, e));
    }
    off_t len = st.st_size;
    char* s = new_cstr(len + 1); // + 1 for the \0 byte
    ssize_t did = read(fd, s, len);
    if (did < 0) {
        int e = errno;
        close(fd);
        free(s);
        return Err(String, SystemError)(systemError(SYSCALLINFO_read, e));
    }
    // do we have to retry? probably. But, should also avoid stat.
    assert(did == len);
    if (close(fd) < 0) {
        int e = errno;
        free(s);
        return Err(String, SystemError)(systemError(SYSCALLINFO_close, e));
    }
    return Ok(String, SystemError)(String(s));
}


#undef RESRET
#undef INIT_RESRET
