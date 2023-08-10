/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#pragma once

#include <cj50/gen/Result.h>
#include <cj50/gen/Option.h>
#include <cj50/CStr.h>
#include <cj50/String.h>
#include <cj50/u32.h>
#include <cj50/os.h>
#include <cj50/gen/dispatch/new_from.h>
#include "cj50/resret.h"


/// A single Unicode code point (a single character, unless it's a
/// code point that combines with other code points to a composed
/// character).

typedef struct ucodepoint {
    uint32_t u32; // would only need 21 bits
} ucodepoint;

// ^ Should this be called UCodepoint ? It's a Copy type, but so what?

#define ucodepoint(cp) \
    ((ucodepoint) { .u32 = (cp) })

static UNUSED
void drop_ucodepoint(UNUSED ucodepoint c) {}

static UNUSED
bool equal_ucodepoint(const ucodepoint *a, const ucodepoint *b) {
    return a->u32 == b->u32;
}

static UNUSED
int print_debug_ucodepoint(const ucodepoint *a) {
    INIT_RESRET;
    RESRET(print_move_cstr("ucodepoint("));
    RESRET(print_u32(a->u32));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}

// ------------------------------------------------------------------

enum DecodingErrorKind {
    DecodingErrorKind_invalid_start_byte,
    DecodingErrorKind_premature_eof,
    DecodingErrorKind_invalid_continuation_byte,
    DecodingErrorKind_invalid_codepoint,
    // XX todo:
    // DecodingErrorKind_unexpected_continuation_byte, // same as DecodingErrorKind_invalid_start_byte?
    // non-continuation byte before the end of the character -> DecodingErrorKind_invalid_continuation_byte ?
    DecodingErrorKind_overlong_encoding,
};

typedef struct DecodingError {
    enum DecodingErrorKind kind;
    union {
        int8_t byte_number;
        uint32_t codepoint; // would only need 21 bits, todo
    };
} DecodingError;

#define DecodingError__invalid_start_byte()             \
    ((DecodingError) {                                  \
        .kind = DecodingErrorKind_invalid_start_byte    \
    })

#define DecodingError_with_byte_number(_kind, bytenum)  \
    ((DecodingError) {                                  \
        .kind = (_kind),                                \
        .byte_number = (bytenum)                        \
    })

#define DecodingError_with_codepoint(cp)                \
    ((DecodingError) {                                  \
        .kind = DecodingErrorKind_invalid_codepoint,    \
        .byte_number = (cp)                             \
    })


// Sick, using bool here since even NORETURN wouldn't make gcc accept
// the void function in boolean context.
static UNUSED
bool die_match_failure() {
    DIE("bug: match failure");
}

static UNUSED
bool equal_DecodingError(const DecodingError *a, const DecodingError *b) {
    return (a->kind == b->kind)
        && ((a->kind == DecodingErrorKind_invalid_start_byte)
            ||
            ((a->kind == DecodingErrorKind_premature_eof) ?
             (a->byte_number == b->byte_number) :
             (a->kind == DecodingErrorKind_invalid_continuation_byte) ?
             (a->byte_number == b->byte_number) :
             (a->kind == DecodingErrorKind_invalid_codepoint) ?
             (a->codepoint == b->codepoint) :
             die_match_failure()));
}

static UNUSED
int print_debug_DecodingError(const DecodingError *e) {
    INIT_RESRET;
    RESRET(printf("DecodingError("));
    switch (e->kind) {
    default: die_match_failure();

    case DecodingErrorKind_invalid_start_byte:
        RESRET(printf("DecodingErrorKind_invalid_start_byte"));
        break;        
    case DecodingErrorKind_premature_eof:
        // well, this is proper ADT approach but not proper C syntax!
        // Except if I make macros. todo?
        RESRET(printf("DecodingErrorKind_premature_eof(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_invalid_continuation_byte:
        RESRET(printf("DecodingErrorKind_invalid_continuation_byte(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_invalid_codepoint:
        RESRET(printf("DecodingErrorKind_invalid_continuation_byte(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_overlong_encoding:
        // XX will it have some info?
        RESRET(printf("DecodingErrorKind_overlong_encoding"));
        break;
    }
    RESRET(printf(")"));
cleanup:
    return ret;
}

static UNUSED
int fprintln_DecodingError(FILE* out, const DecodingError* e) {
    INIT_RESRET;
    RESRET(fprintf(out, "UTF-8 decoding error: "));
    switch (e->kind) {
    default: die_match_failure();

    case DecodingErrorKind_invalid_start_byte:
        RESRET(fprintf(out, "invalid start byte"));
        break;
    case DecodingErrorKind_premature_eof:
        RESRET(fprintf(out, "premature EOF decoding UTF-8 (byte #%i)",
                       e->byte_number));
        break;
    case DecodingErrorKind_invalid_continuation_byte:
        RESRET(fprintf(out, "invalid continuation byte (byte #%i)",
                       e->byte_number));
        break;
    case DecodingErrorKind_invalid_codepoint:
        RESRET(fprintf(out, "invalid code point %ui",
                       e->codepoint));
        break;
    case DecodingErrorKind_overlong_encoding:
        // XX will this have additional info?
        RESRET(fprintf(out, "overlong encoding"));
        break;
    }
    RESRET(fprintf(out, "\n"));
cleanup:
    return ret;
}

static UNUSED
void drop_DecodingError(UNUSED DecodingError e) {}


enum UnicodeErrorKind {
    UnicodeErrorKind_SystemError,
    UnicodeErrorKind_DecodingError
};

typedef struct UnicodeError {
    enum UnicodeErrorKind kind;
    union {
        SystemError systemError;
        DecodingError decodingError;
    };
} UnicodeError;


static UNUSED
UnicodeError new_UnicodeError_from_SystemError(SystemError e) {
    return (UnicodeError) {
        .kind = UnicodeErrorKind_SystemError,
        .systemError = e
    };
}

static UNUSED
UnicodeError new_UnicodeError_from_DecodingError(DecodingError e) {
    return (UnicodeError) {
        .kind = UnicodeErrorKind_DecodingError,
        .decodingError = e
    };
}

static UNUSED
UnicodeError new_UnicodeError_from_UnicodeError(UnicodeError e) {
    return e;
}


// See "sick" above.
static UNUSED
bool die_invalid_UnicodeErrorKind(enum UnicodeErrorKind kind) {
    DIE_("bug: invalid UnicodeErrorKind %i", kind);
}

static UNUSED
bool equal_UnicodeError(const UnicodeError *a, const UnicodeError *b) {
    return (a->kind == b->kind)
        && ((a->kind == UnicodeErrorKind_SystemError) ?
            equal_SystemError(&a->systemError, &b->systemError) :
            (a->kind == UnicodeErrorKind_DecodingError) ?
            equal_DecodingError(&a->decodingError, &b->decodingError) :
            die_invalid_UnicodeErrorKind(a->kind));
}

static UNUSED
int print_debug_UnicodeError(const UnicodeError *e) {
    INIT_RESRET;
    RESRET(printf("UnicodeError("));
    switch (e->kind) {
    default: die_match_failure();

    case UnicodeErrorKind_SystemError:
        return print_debug_SystemError(&e->systemError);
    case UnicodeErrorKind_DecodingError:
        return print_debug_DecodingError(&e->decodingError);
    }
    RESRET(printf(")"));
cleanup:
    return ret;
}

static UNUSED
int fprintln_UnicodeError(FILE* out, const UnicodeError* e) {
    switch (e->kind) {
    default: die_match_failure();

    case UnicodeErrorKind_SystemError:
        return fprintln_SystemError(out, &e->systemError);
    case UnicodeErrorKind_DecodingError:
        return fprintln_DecodingError(out, &e->decodingError);
    }
}

static UNUSED
void drop_UnicodeError(UnicodeError e) {
    switch (e.kind) {
    default: die_match_failure();

    case UnicodeErrorKind_SystemError:
        drop_SystemError(e.systemError);
        break;
    case UnicodeErrorKind_DecodingError:
        drop_DecodingError(e.decodingError);
        break;
    }
}

// ------------------------------------------------------------------

/// Encode a unicode code point as UTF-8 characters, writing it to
/// `out`. `out` must have 4 bytes of storage or more. No `'\0'` byte
/// is written afterwards. Returns -1 if `cp` is not a valid unicode
/// codepoint, otherwise returns the number of bytes written.

static UNUSED
int encode_utf8(uint32_t cp, uint8_t *out) {
    if (cp <= 0x7F) {
        out[0] = cp & 0x7F;
        return 1;
    }
    // The lower bits appear later in the encoding!
    if (cp <= 0x7FF) {
        out[0] = (cp >> 6) | 0b11000000;
        out[1] = (cp & 0b00111111) | 0b10000000;
        return 2;
    }
    if (cp <= 0xFFFF) {
        out[0] = (cp >> 12) | 0b11100000;
        out[1] = ((cp >> 6) & 0b00111111) | 0b10000000;
        out[2] = (cp & 0b00111111) | 0b10000000;
        return 3;
    }
    if (cp <= 0x10FFFF) {
        out[0] = (cp >> 18) | 0b11110000;
        out[1] = ((cp >> 12) & 0b00111111) | 0b10000000;
        out[2] = ((cp >> 6) & 0b00111111) | 0b10000000;
        out[3] = (cp & 0b00111111) | 0b10000000;
        return 4;
    }
    return -1;
    // XX weren't there holes in validity, too?
}

/// How many bytes the UTF-8 character sequence takes when `b` is its
/// initial byte. None is returned if `b` is not ascii or an initial
/// byte, but a continuation byte.

static UNUSED
Option(u8) utf8_sequence_len(u8 b) {
    if (b <= 0x7F) { return some_u8(1); }
    if ((b & 0b11100000) == 0b11000000) { return some_u8(2); }
    if ((b & 0b11110000) == 0b11100000) { return some_u8(3); }
    if ((b & 0b11111000) == 0b11110000) { return some_u8(4); }
    return none_u8();
}

// ------------------------------------------------------------------


/// A single Unicode code point in UTF-8 format.

typedef struct utf8char {
    // 1..4 bytes data, then a \0 char, and a len indicator
    uint8_t data[6];
} utf8char;

/// Careful, currently unsafe, assumes that the str is a string
/// constant and carefully entered!
#define utf8char(str)                          \
    new_utf8char_from_cstr_unsafe(str)

/// Create utf8char from bytes and length of the UTF-8 sequence. No
/// safety checks whatsoever are done.
static UNUSED
utf8char new_utf8char_from_bytes_seqlen_unsafe(const char *bytes,
                                               u8 seqlen) {
    utf8char c;
    memcpy(c.data, bytes, seqlen);
    c.data[seqlen] = '\0';
    c.data[5] = seqlen;
    return c;
}

/// Create utf8char from bytes and length of the UTF-8 sequence. No
/// safety checks whatsoever are done.
static UNUSED
utf8char new_utf8char_from_cstr_unsafe(cstr s) {
    return new_utf8char_from_bytes_seqlen_unsafe(s,
                                                 strlen(s));
}


/// The length of the UTF-8 byte sequence making up the given unicode
/// codepoint.
static UNUSED
size_t len_utf8char(utf8char c) {
    return c.data[5];
}

/// A cstr borrowed from the data in `c`.
static UNUSED
cstr to_cstr_utf8char(const utf8char *c) {
    return (cstr)c->data;
}

static UNUSED
void drop_utf8char(UNUSED utf8char c) {}

static UNUSED
bool equal_utf8char(const utf8char *a, const utf8char *b) {
    return ((len_utf8char(*a) == len_utf8char(*b)) &&
            memcmp(a->data, b->data, len_utf8char(*a)) == 0);
}

static UNUSED
bool equal_move_utf8char(utf8char a, utf8char b) {
    return equal_utf8char(&a, &b);
}

static UNUSED
int print_utf8char(const utf8char *c) {
    INIT_RESRET;
    RESRET(print_move_cstr(to_cstr_utf8char(c)));
cleanup:
    return ret;
}

static UNUSED
int print_move_utf8char(utf8char c) {
    return print_utf8char(&c);
}

static UNUSED
int print_debug_utf8char(const utf8char *c) {
    INIT_RESRET;
    RESRET(print_move_cstr("utf8char(")); // XX use something executable please
    RESRET(print_debug_move_cstr(to_cstr_utf8char(c)));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


GENERATE_Option(utf8char);

static UNUSED
Option(utf8char) new_utf8char_from_ucodepoint(ucodepoint cp) {
    utf8char c;
    int len = encode_utf8(cp.u32, c.data);
    if (len < 0) {
        return none_utf8char();
    }
    c.data[len] = '\0';
    c.data[5] = len;
    return some_utf8char(c);
}

// ------------------------------------------------------------------


GENERATE_Option(ucodepoint);

GENERATE_Result(Option(ucodepoint), UnicodeError);


/// Read a single Unicode code point from `in`.

/// This function is currently hard-coded to decode files in the UTF-8
/// format.

static UNUSED
Result(Option(ucodepoint), UnicodeError) get_ucodepoint_unlocked(
    CFile *in)
{
    BEGIN_Result(Option(ucodepoint), UnicodeError);
    
    // https://en.wikipedia.org/wiki/Utf-8#Encoding
#define EBUFSIZ 256
    u32 codepoint;
    AUTO opt_b1 = TRY(os_getc_unlocked(in), cleanup);
    LET_Some_ELSE(b1, opt_b1) {
        RETURN_Ok(none_ucodepoint(), cleanup);
    }
    if ((b1 & 128) == 0) {
        // codepoint encoded as a single byte
        codepoint = b1;
    } else {
        int numbytes;
        if        ((b1 & 0b11100000) == 0b11000000) {
            numbytes = 2;
            codepoint = b1 & 0b11111;
        } else if ((b1 & 0b11110000) == 0b11100000) {
            numbytes = 3;
            codepoint = b1 & 0b1111;
        } else if ((b1 & 0b11111000) == 0b11110000) {
            numbytes = 4;
            codepoint = b1 & 0b111;
        } else {
            RETURN_Err(new_from(UnicodeError,
                                DecodingError__invalid_start_byte()),
                       cleanup);
        }
        for (int i = 1; i < numbytes; i++) {
            AUTO opt_b = TRY(os_getc_unlocked(in), cleanup);
            LET_Some_ELSE(b, opt_b) {
                RETURN_Err(
                    new_from(UnicodeError,
                             DecodingError_with_byte_number(
                                 DecodingErrorKind_premature_eof,
                                 i+1)),
                    cleanup);
            }
            if ((b & 0b11000000) != 0b10000000) {
                RETURN_Err(
                    new_from(UnicodeError,
                             DecodingError_with_byte_number(
                                 DecodingErrorKind_invalid_continuation_byte,
                                 i+1)),
                    cleanup);
            }
            codepoint <<= 6;
            codepoint |= (b & 0b00111111);
        }
    }
    if (codepoint <= 0x10FFFF) {
        RETURN_Ok(some_ucodepoint(ucodepoint(codepoint)),
                  cleanup);
    } else {
        RETURN_Err(new_from(UnicodeError,
                            DecodingError_with_codepoint(codepoint)),
                   cleanup);
    }
cleanup:
    END_Result();
#undef EBUFSIZ
}


// ------------------------------------------------------------------
// Operations for String

/// Get the character (unicode codepoint, to be precise) at byte
/// position `idx` of `s`, if possible. Failures can be because `idx`
/// is at or behind the end of the string contents, or because it does
/// not point to the beginning of a byte sequence for a UTF-8 encoded
/// codepoint.

static UNUSED
Option(utf8char) get_utf8char_String(const String *s, size_t idx) {
    size_t len = s->vec.len;
    char *ptr = s->vec.ptr;
    if (idx < len) {
        if_let_Some(seqlen, utf8_sequence_len(ptr[idx])) {
            assert((idx + seqlen) <= len); // String guarantees UTF-8
            return some_utf8char(
                new_utf8char_from_bytes_seqlen_unsafe(
                    &ptr[idx], seqlen));
        } else_None {
            // middle of UTF-8 sequence
            return none_utf8char();
        }
    } else {
        // past the end
        return none_utf8char();
    }
}


#include <cj50/instantiations/Result_Vec_ucodepoint__UnicodeError.h>

/// Convert a `cstr` into a vector of unicode codepoints, if possible.
/// Conversion failures due to invalid UTF-8 are reported.

static UNUSED
Result(Vec(ucodepoint), UnicodeError) new_Vec_ucodepoint_from_cstr(cstr s)
{
    BEGIN_Result(Vec(ucodepoint), UnicodeError);

    CFile in = TRY(memopen_CFile((char*)s, strlen(s), "r"), cleanup1);
    Vec(ucodepoint) v = new_Vec_ucodepoint();
    while_let_Some(c, TRY(get_ucodepoint_unlocked(&in), cleanup2)) {
        push_Vec_ucodepoint(&v, c);
    }
    RETURN_Ok(v, cleanup2);
cleanup2:
    drop_CFile(in);
cleanup1:
    END_Result();
}

#include <cj50/instantiations/Result_Vec_utf8char__UnicodeError.h>

static UNUSED
void die_bug_unicode() {
    DIE("bug in unicode.h");
}

/// Convert a `cstr` into a vector of unicode codepoints, if possible.
/// Conversion failures due to invalid UTF-8 are reported.

static UNUSED
Result(Vec(utf8char), UnicodeError) new_Vec_utf8char_from_cstr(cstr s)
{
    BEGIN_Result(Vec(utf8char), UnicodeError);

    // Go the 'cheap' route of converting to Vec(ucodepoint) first.
    Vec(ucodepoint) v0 = TRY(new_Vec_ucodepoint_from_cstr(s), cleanup1);
    Vec(utf8char) v1 = new_Vec_utf8char();

    // Should have FOR_EACH syntax
    for (size_t i = 0; i < v0.len; i++) {
        // Since ucodepoint is Copy, we don't need to take it by ref
        if_let_Some(uc, new_utf8char_from_ucodepoint(v0.ptr[i])) {
            push_Vec_utf8char(&v1, uc);
        } else_None {
            // should never happen, since UTF-8 decoding above would
            // have reported any issue already.
            die_bug_unicode();
        }
    }
    RETURN_Ok(v1, cleanup2);

cleanup2:
    drop_Vec_ucodepoint(v0);
cleanup1:
    END_Result();
}

