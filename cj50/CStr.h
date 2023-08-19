#pragma once

#include <stdint.h>
#include <cj50/gen/Option.h>
#include <cj50/gen/Result.h>
#include <cj50/char.h>
#include <cj50/xmem.h>
#include <cj50/gen/error.h>
#include <cj50/resret.h>


/// `cstr` is a non-mutable borrowed type that represents a "C
/// string". It is an array of `char`, that represents the text and a
/// '\0' character after it to signal the end of the text. There is no
/// length information, the length has to be determined by walking the
/// array until encountering the '\0' character (`strlen` will do
/// that).
typedef const char* cstr;

// even though not owned, we need this because Option refers to it
static UNUSED
void drop_cstr(UNUSED cstr s) {}

/// Check equivalence. (Standard borrowing API, even though cstr is
/// already a reference type.)
static UNUSED
bool equal_cstr(const cstr *a, const cstr *b) {
    return strcmp(*a, *b) == 0;
}

/// Check equivalence, "consuming" the references, although consuming
/// references is a NOOP (those are Copy, and the referenced value is
/// not dropped). Offered to satisfy the expectation that one can
/// compare reference types without using `&`.
static UNUSED
bool equal_move_cstr(cstr a, cstr b) {
    return strcmp(a, b) == 0;
}

/// Print for program user.
static UNUSED
int print_cstr(const cstr *s) {
    return printf("%s", *s);
}

/// Same as `print_cstr`, but move (rather, copy) the reference. Like
/// print_cstr, still does not consume `s` since it's a borrowed type
/// anyway.
static UNUSED
int print_move_cstr(cstr s) {
    return print_cstr(&s);
}


/// Print for program user.
static UNUSED
int fprintln_cstr(FILE *out, const cstr *s) {
    return fprintf(out, "%s\n", *s);
}

static UNUSED
int fprintln_move_cstr(FILE *out, cstr s) {
    return fprintln_cstr(out, &s);
}


/// Print in C code syntax.
static
int print_debug_cstr(const cstr *s) {
    INIT_RESRET;
    RESRET(print_move_cstr("\""));

    cstr str = *s;
    while (*str) {
        RESRET(_print_debug_char(*str));
        str++;
    }
    RESRET(print_move_cstr("\""));
cleanup:
    return ret;
}

/// Same as `print_debug_move_cstr`, again move/copy but it's a
/// borrowed type anyway.
static UNUSED
int print_debug_move_cstr(cstr s) {
    return print_debug_cstr(&s);
}


GENERATE_Option(cstr);



/// `CStr` is a mutable owned type that represents a "C string". It
/// embeds an array of `char`, that represents the text and a '\0'
/// character after it to signal the end of the text. There is no
/// length information, the length has to be determined by walking the
/// array until encountering the '\0' character (`strlen` will do
/// that). In addition to `cstr` it guarantees that there are no '\0'
/// characters contained in the text (functions generating `CStr` must
/// verify for that to be true).
typedef struct CStr {
    char* cstr;
} CStr;

/// Generate a `CStr` from a `cstr`. `cstr` must end with a '\0' and
/// not contain any '\0' in other places; this is not checked, hence
/// unsafe.
#define CStr_from_cstr_unsafe(s)                \
    ((CStr) { .cstr = s })


/// Remove from existence.
static UNUSED
void drop_CStr(CStr s) {
    free(s.cstr);
}

/// Check equivalence.
static UNUSED
bool equal_CStr(const CStr *a, const CStr *b) {
    return strcmp(a->cstr, b->cstr) == 0;
}

/// Print in C code syntax.
static UNUSED
int print_debug_CStr(const CStr *s) {
    return print_debug_cstr((const cstr*)&s->cstr);
}

/// Same as `print_debug_CStr` but consuming `s`.
static UNUSED
int print_debug_move_CStr(CStr s) {
    int res = print_debug_cstr((const cstr*)&s.cstr);
    drop_CStr(s);
    return res;
}


GENERATE_Option(CStr);

// ------------------------------------------------------------------
// Errors

/// A type indicating an error handling C strings. The instances are:

///     CStrError_MissingTerminator
///     CStrError_ContainsNul
///     CStrError_Size0

typedef struct CStrError {
    uint8_t code;
} CStrError;

#define CStrError(cod)                          \
    ((CStrError) { .code = cod })

/// Check equivalence.
static
bool equal_CStrError(const CStrError *a, const CStrError *b) {
    return a->code == b->code;
}

#define DEF_CStrError(code, name) const CStrError name = CStrError(code)

// Duplication: also see the docstring for struct CStrError

DEF_CStrError(1, CStrError_MissingTerminator);
DEF_CStrError(2, CStrError_ContainsNul);
DEF_CStrError(3, CStrError_Size0);

const struct constant_name_and_message _CSE_and_message_from_CStrError_code[] = {
    { NULL, NULL},
    { "CStrError_MissingTerminator", "char array is missing '\\0' terminator" },
    { "CStrError_ContainsNul", "C string contains '\\0' before the end" },
    { "CStrError_Size0", "char array of size 0 cannot be a C string" },
};
#define _CSE_and_message_from_CStrError_code_len        \
    (sizeof(_CSE_and_message_from_CStrError_code)       \
     / sizeof(struct constant_name_and_message))

#undef DEF_CStrError
// Make final (well)
#undef CStrError

/// Print in C code syntax.
static UNUSED
int print_debug_CStrError(const CStrError *e) {
    // (We said final, and now we're printing syntax using the constructor!...)
    assert(e->code < _CSE_and_message_from_CStrError_code_len);
    return printf("CStrError(%s)",
                  _CSE_and_message_from_CStrError_code[e->code].constant_name);
}

/// Print for program user.
static
int fprintln_CStrError(FILE *out, const CStrError *e) {
    assert(e->code < _CSE_and_message_from_CStrError_code_len);
    return fprintf(
        out, "CStr error: %s\n",
        _CSE_and_message_from_CStrError_code[e->code].message);
}

static UNUSED
void drop_CStrError(UNUSED CStrError e) {}

// ------------------------------------------------------------------


GENERATE_Result(CStr, CStrError);

/// Consume `s`, verify that it ends with a '\0' but does not contain
/// any '\0' elsewhere. `siz` is the length including the '\0'
/// terminator.
static UNUSED
Result(CStr, CStrError) cStr_from_cstr(char *s, size_t siz) {
    if (siz == 0) {
        return Err(CStr, CStrError)(CStrError_Size0);
    }
    if (s[siz - 1] != '\0') {
        return Err(CStr, CStrError)(CStrError_MissingTerminator);
    }
    for (size_t i = 0; i < siz - 1; i++) {
        if (s[i] == '\0') {
            return Err(CStr, CStrError)(CStrError_ContainsNul);
        }
    }
    return Ok(CStr, CStrError)(CStr_from_cstr_unsafe(s));
}

/// Allocate space for a C string of capacity `len`. Aborts when there
/// is not enough memory. All slots are set to '\0' (the string starts
/// out as the empty string).

/// NOTE: This means that the contained cstr is *not* a '\0'-less
/// string as required by `cStr_from_cstr` with regards to `len`. It
/// is shortened to the first '\0' encountered. Mutating the contained
/// cstr is *unsafe* (nothing except your being careful is preventing
/// you from writing behind the end of the contained cstr (buffer
/// overflow), and nothing is preventing you from filling it with
/// non-'\0' characters till the end meaning it loses its terminator).
static
CStr new_CStr(size_t len) {
    return CStr_from_cstr_unsafe(xcallocarray(len, 1));
}

/// Print a CStr (without any escaping) to stdout.
static UNUSED
int print_CStr(const CStr *s) {
    return print_move_cstr(s->cstr);
}

/// Like print_CStr but consuming the argument.
static UNUSED
int print_move_CStr(CStr s) {
    int res = print_CStr(&s);
    drop_CStr(s);
    return res;
}


/// Give a read-only borrow to the contained cstr.
static UNUSED
cstr cstr_CStr(CStr *s) {
    return s->cstr;
}


/// "Dereference" the `CStr` wrapper, meaning, get the wrapped value
/// out, a `cstr`.
static UNUSED
cstr deref_CStr(const CStr *s) {
    return s->cstr;
}

