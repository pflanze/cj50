#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cj50/os.h>
#include <cj50/CStr.h>
#include <cj50/String.h>
#include <cj50/gen/Result.h>
#include <cj50/gen/Option.h>
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
/// a String, if possible (no system errors occurred). If the file
/// contains more than `maxlen` unicode codepoints, an error with
/// `.kind == UnicodeErrorKind_LimitExceededError` is returned.

static UNUSED
Result(String, UnicodeError) filecontents_String(cstr path, size_t max_len) {
    BEGIN_Result(String, UnicodeError);

    CFile in = TRY(open_CFile(path, "r"), cleanup0);
    String s = new_String();

    size_t nread = 0;
    while_let_Some(cp, TRY(get_ucodepoint_unlocked_CFile(&in), cleanup1)) {
        nread++;
        if (nread < max_len) {
            push_ucodepoint_String(&s, cp);
        } else {
            RETURN_Err(UnicodeError_LimitExceeded, cleanup1);
        }
    }
    RETURN_Ok(s, cleanup1);

cleanup1:
    TRY(close_CFile(&in), cleanup0);
    drop_CFile(in);
cleanup0:
    END_Result();
}


