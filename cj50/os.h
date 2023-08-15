#pragma once

//! Wrappers around C library and POSIX functions using `Result`
//! instead of the traditional combination of in-band error signalling
//! and `errno`.

#include <unistd.h> /* fsync, fdatasync, .. */

#include <cj50/syscallinfo.h> /* rename to oscallinfo ? */
#include <cj50/u8.h>
#include <cj50/int.h>
#include "cj50/resret.h"



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
    RESRET(print_move_int(v->oserror.number)); // todo: look up constant names like ENOPERM etc.?
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


// used in Result
static UNUSED
int fprintln_SystemError(FILE* out, const SystemError *e) {
    INIT_RESRET;
    RESRET(fprintf(out, "system error: %s(%i): %s\n",
                   syscallinfos[e->syscallinfo_id].name,
                   syscallinfos[e->syscallinfo_id].manpage_section,
                   strerror(e->oserror.number)));
cleanup:
    return ret;
}

static UNUSED
void drop_SystemError(UNUSED SystemError e) {}

static UNUSED
SystemError new_SystemError_from_SystemError(SystemError e) {
    return e;
}

// ------------------------------------------------------------------ 

/// An owned type holding a C library `FILE*` type. The contained
/// pointer is never `NULL` except after calling `close_File`.

typedef struct CFile {
    FILE *ptr;
} CFile;

#define CFile(_ptr)                              \
    ((CFile) { .ptr = _ptr })


/// Equality on CFile does not make much sense; it does report whether
/// the embedded `FILE*` pointers are identical.

static UNUSED
bool equal_CFile(const CFile *a, const CFile *b) {
    return a->ptr == b->ptr; // hmmmm.
}

static UNUSED
int print_debug_CFile(const CFile *v) {
    INIT_RESRET;
    RESRET(printf("CFile(%p)", v->ptr));
cleanup:
    return ret;
}

/// Closes the file.

/// This does not report errors (except it does print a warning),
/// because the drop interface can't, except via abort. This is
/// exactly like in Rust. IIRC the conclusion there was that modern
/// systems don't ever report errors on `close` any more, except
/// perhaps some networked file systems like NFS or wrongly made fuse
/// file systems. To be sure that there was no problem writing, either
/// call `fclose(f.ptr)` or `flush(&f)` and handle the errors
/// there. (Also, `sync(&f)`.)

static UNUSED
void drop_CFile(CFile f) {
    if (f.ptr) {
        if (fclose(f.ptr) != 0) {
            WARN_("Warning: drop_CFile: fclose failed: %s",
                  strerror(errno));
        }
    }
}

// ------------------------------------------------------------------ 

GENERATE_Result(CFile, SystemError);
GENERATE_Result(Unit, SystemError);
GENERATE_Result(Option(u8), SystemError);


// ------------------------------------------------------------------ 

/// Returns a single byte from the input, if possible; at EOF (end of
/// file), returns Ok(None). `inp` must previously have been locked
/// using `flockfile` and afterwards unlocked using `funlockfile` (see
/// `man 3 flockfile`).
static UNUSED
Result(Option(u8), SystemError) os_getc_unlocked(CFile *inp) {
    int r = getc_unlocked(inp->ptr);
    if (r == EOF) {
        if (ferror(inp->ptr)) {
            return Err(Option(u8), SystemError)(
                systemError(SYSCALLINFO_getc_unlocked, errno));
        } else {
            return Ok(Option(u8), SystemError)(none_u8());
        }
    } else {
        return Ok(Option(u8), SystemError)(some_u8(r));
    }
}


/// Opens the file whose name is the string pointed to by `pathname` and
/// associates a stream with it.

/// For details, including the meaning of `mode`, see `man 3 fopen`.

static UNUSED
Result(CFile, SystemError) open_CFile(cstr pathname, cstr mode) {
    FILE *f = fopen(pathname, mode);
    if (f) {
        return Ok(CFile, SystemError)(CFile(f));
    } else {
        return Err(CFile, SystemError)(
            systemError(SYSCALLINFO_fopen, errno));
    }
}

/// Opens a stream that permits the access specified by mode.  The
/// stream allows I/O to be performed on the string or memory buffer
/// pointed to by buf.

/// For details, see `man 3 fmemopen`.

static UNUSED
Result(CFile, SystemError) memopen_CFile(void *buf, size_t size, cstr mode) {
    FILE *f = fmemopen(buf, size, mode);
    if (f) {
        return Ok(CFile, SystemError)(CFile(f));
    } else {
        return Err(CFile, SystemError)(
            systemError(SYSCALLINFO_fmemopen, errno));
    }
}


/// For output streams, forces a write of all user-space buffered data
/// for the given output.

/// For input streams associated with seekable files (e.g., disk
/// files, but not pipes or terminals), discards any buffered data
/// that has been fetched from the underlying file, but has not been
/// consumed by the application.

/// The open status of the stream is unaffected.

static UNUSED
Result(Unit, SystemError) flush_CFile(CFile *f) {
    assert(f->ptr);
    if (fflush(f->ptr) == 0) {
        return Ok(Unit, SystemError)(Unit());
    } else {
        return Err(Unit, SystemError)(
            systemError(SYSCALLINFO_fflush, errno));
    }
}


/// Transfers ("flushes") all modified in-core data of (i.e., modified
/// buffer cache pages for) the file referred to by the file
/// descriptor fd to the disk device (or other permanent storage de‐
/// vice) so that all changed information can be retrieved even if the
/// system crashes or is rebooted.  This includes writing through or
/// flushing a disk cache if present.  The call blocks until the
/// device reports that the transfer has completed.
/// 
/// As well as flushing the file data, also flushes the
/// metadata information associated with the file (see inode(7)).

static UNUSED
Result(Unit, SystemError) sync_CFile(CFile *f) {
    assert(f->ptr);
    int fd = fileno(f->ptr);
    if (fsync(fd) == 0) {
        return Ok(Unit, SystemError)(Unit());
    } else {
        return Err(Unit, SystemError)(
            systemError(SYSCALLINFO_fsync, errno));
    }
}


static UNUSED
Result(Unit, SystemError) datasync_CFile(CFile *f) {
    assert(f->ptr);
    int fd = fileno(f->ptr);
    if (fdatasync(fd) == 0) {
        return Ok(Unit, SystemError)(Unit());
    } else {
        return Err(Unit, SystemError)(
            systemError(SYSCALLINFO_fdatasync, errno));
    }
}



/// Close the file. Marks `f` so that the embedded `FILE*` pointer is
/// `NULL`. It is safe to call `drop` afterwards, but any other
/// function will segfault (NULL pointer dereference)!

static UNUSED
Result(Unit, SystemError) close_CFile(CFile *f) {
    if (fclose(f->ptr) == 0) {
        f->ptr = NULL;
        return Ok(Unit, SystemError)(Unit());
    } else {
        return Err(Unit, SystemError)(
            systemError(SYSCALLINFO_fclose, errno));
    }
}

