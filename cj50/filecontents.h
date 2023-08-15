#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cj50/os.h>
#include <cj50/CStr.h>
#include <cj50/String.h>
#include <cj50/gen/Result.h>
#include <cj50/unicode.h>
#include <cj50/instantiations/Result_String__UnicodeError.h>

static
int print_int(int n);


// ------------------------------------------------------------------ 

GENERATE_Result(String, SystemError);

GENERATE_Result(CStr, SystemError);

// ^ XX move both out; not actually used here! (But added generics
// already, right?)


/// Returns a copy of the contents of the file at the given `path` as
/// a String, if possible (no system errors occurred).

static UNUSED
Result(String, UnicodeError) filecontents_String(cstr path) {
    BEGIN_Result(String, UnicodeError);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        RETURN_Err(systemError(SYSCALLINFO_open, errno), cleanup0);
    }
    struct stat st;
    if (fstat(fd, &st) < 0) {
        RETURN_Err(systemError(SYSCALLINFO_fstat, errno), cleanup1);
    }
    off_t len = st.st_size;

    // XX todo: instead iteratively append to String s = new_String();
    String buf = with_capacity_String(len + 1); // +1 spare for potential \0

    ssize_t did = read(fd, buf.vec.ptr, buf.vec.cap);
    if (did < 0) {
        RETURN_Err(systemError(SYSCALLINFO_read, errno), cleanup2);
    }
    // do we have to retry? probably. But, should also avoid stat.
    assert(did == len);
    buf.vec.len = did;
    RETURN_Ok(buf, cleanup1);

cleanup2:
    drop_String(buf);
cleanup1:
    if (close(fd) < 0) {
        RETURN_Err(systemError(SYSCALLINFO_close, errno), cleanup0);
    }
cleanup0:
    END_Result();
}


