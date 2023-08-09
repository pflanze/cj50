#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cj50/os.h>
#include <cj50/CStr.h>
#include <cj50/gen/Result.h>

static
int print_int(int n);


// ------------------------------------------------------------------ 

// XX for future when going 'back' to using String
GENERATE_Result(String, SystemError);

GENERATE_Result(CStr, SystemError);

// XXX todo: use a string type with len so that \0 will not be lost

/// Returns a copy of the contents of the file at the given `path` as
/// a String, if possible (no system errors occurred).

/// XXX Note: the String has a '\0' terminator character added. Any '\0'
/// characters contained in the file will be part of the returned
/// String and lead to it being interpreted as terminating there.

static UNUSED
Result(CStr, SystemError) filecontents_CStr(cstr path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        int e = errno;
        return Err(CStr, SystemError)(systemError(SYSCALLINFO_open, e));
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        int e = errno;
        close(fd);
        return Err(CStr, SystemError)(systemError(SYSCALLINFO_fstat, e));
    }
    off_t len = st.st_size;
    CStr s = new_CStr(len + 1); // + 1 for the \0 byte
    ssize_t did = read(fd, s.cstr, len);
    if (did < 0) {
        int e = errno;
        close(fd);
        drop_CStr(s);
        return Err(CStr, SystemError)(systemError(SYSCALLINFO_read, e));
    }
    // do we have to retry? probably. But, should also avoid stat.
    assert(did == len);
    if (close(fd) < 0) {
        int e = errno;
        drop_CStr(s);
        return Err(CStr, SystemError)(systemError(SYSCALLINFO_close, e));
    }
    return Ok(CStr, SystemError)(s);
}


