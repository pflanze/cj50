#pragma once

#include <cj50/gen/Option.h>
#include <cj50/gen/Result.h>
#include <cj50/char.h>
#include <cj50/xmem.h>


#define RESRET(e)                \
    res = (e);                   \
    if (res < 0) { return res; } \
    ret += res;


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

// receive pointers to pointers just for standard in Option
static UNUSED
bool equal_cstr(const cstr *a, const cstr *b) {
    return strcmp(*a, *b) == 0;
}

static UNUSED
bool equal_move_cstr(cstr a, cstr b) {
    return strcmp(a, b) == 0;
}


int print_cstr(cstr str) {
    return printf("%s", str);
}

static
int print_debug_cstr(const cstr *s) {
    int ret = 0;
    int res;
    RESRET(print_cstr("\""));

    cstr str = *s;
    while (*str) {
        RESRET(_print_debug_char(*str));
        str++;
    }
    RESRET(print_cstr("\""));
    return ret;
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
#define CStr_from_cstr_unsafe(s) ((CStr) { .cstr = s })

static UNUSED
void drop_CStr(CStr s) {
    free(s.cstr);
}

static UNUSED
bool equal_CStr(const CStr *a, const CStr *b) {
    return strcmp(a->cstr, b->cstr) == 0;
}

static UNUSED
int print_debug_CStr(const CStr *s) {
    return print_debug_cstr((const cstr*)&s->cstr);
}


GENERATE_Option(CStr);

typedef struct CStrError {
    uint8_t code;
} CStrError;

#define CStrError(cod) ((CStrError) { .code = cod })

static
bool equal_CStrError(const CStrError *a, const CStrError *b) {
    return a->code == b->code;
}

struct CSE_and_message {
    const char* cse; // constant name
    const char* message;
};

#define DEF_CStrError(code, name) const CStrError name = CStrError(code)

DEF_CStrError(1, CSE_missing_terminator);
DEF_CStrError(2, CSE_contains_nul);
DEF_CStrError(3, CSE_size_0);

const struct CSE_and_message _CSE_and_message_from_CStrError_code[] = {
    { NULL, NULL},
    { "CSE_missing_terminator", "char array is missing '\\0' terminator" },
    { "CSE_contains_nul", "C string contains '\\0' before the end" },
    { "CSE_size_0", "char array of size 0 cannot be a C string" },
};
#define _CSE_and_message_from_CStrError_code_len \
    (sizeof(_CSE_and_message_from_CStrError_code) / sizeof(struct CSE_and_message))

#undef DEF_CStrError
// Make final (well)
#undef CStrError

static UNUSED
int print_debug_CStrError(const CStrError *e) {
    // (We said final, and now we're printing syntax using the constructor!...)
    assert(e->code < _CSE_and_message_from_CStrError_code_len);
    return printf("CStrError(%s)",
                  _CSE_and_message_from_CStrError_code[e->code].cse);
}

static
int fprintln_CStrError(FILE *out, CStrError e) {
    assert(e.code < _CSE_and_message_from_CStrError_code_len);
    return fprintf(out, "CStr error: %s\n",
                   _CSE_and_message_from_CStrError_code[e.code].message);
}


GENERATE_Result(CStr, CStrError);

/// Consume `s`, verify that it ends with a '\0' but does not contain
/// any '\0' elsewhere. `siz` is the length including the '\0'
/// terminator.
static UNUSED
Result(CStr, CStrError) cStr_from_cstr(char *s, size_t siz) {
    if (siz == 0) {
        return Err(CStr, CStrError)(CSE_size_0);
    }
    if (s[siz - 1] != '\0') {
        return Err(CStr, CStrError)(CSE_missing_terminator);
    }
    for (size_t i = 0; i < siz - 1; i++) {
        if (s[i] == '\0') {
            return Err(CStr, CStrError)(CSE_contains_nul);
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


static UNUSED
int print_CStr(const CStr *s) {
    return print_cstr(s->cstr);
}


#undef RESRET

