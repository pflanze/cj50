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
#include <cj50/resret.h>
#include <cj50/unicodeError.h>


static UNUSED
void die_bug_unicode() {
    DIE("bug in unicode.h");
}


/// A single Unicode code point (a single character, unless it's a
/// code point that combines with other code points to a composed
/// character). ucodepoint only ever carries valid codes.

typedef struct ucodepoint {
    uint32_t u32; // would only need 21 bits
} ucodepoint;

// ^ Should this be called UCodepoint ? It's a Copy type, but so what?

/// Unsafe constructor, does not verify the code.
#define ucodepoint(cp) \
    ((ucodepoint) { .u32 = (cp) })

// XX todo: add safe constructor!

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
cstr cstr_utf8char(const utf8char *c) {
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
    RESRET(print_move_cstr(cstr_utf8char(c)));
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
    RESRET(print_debug_move_cstr(cstr_utf8char(c)));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


GENERATE_Option(utf8char);

/// Convert a ucodepoint to a utf8char.

static UNUSED
utf8char new_utf8char_from_ucodepoint(ucodepoint cp) {
    utf8char c;
    int len = encode_utf8(cp.u32, c.data);
    if (len < 0) {
        // should never happen because ucodepoint should never accept
        // invalid codes
        die_bug_unicode();
    }
    c.data[len] = '\0';
    c.data[5] = len;
    return c;
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

    // XX 'cheap' route
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
        push_Vec_utf8char(&v1,
                          new_utf8char_from_ucodepoint(v0.ptr[i]));
    }
    RETURN_Ok(v1, cleanup2);

cleanup2:
    drop_Vec_ucodepoint(v0);
cleanup1:
    END_Result();
}


/// Appends the given codepoint in utf8char format to the end of this
/// String.

static UNUSED
void push_utf8char_String(String *s, utf8char c) {
    // (XX todo: there should be something in Vec to add multiple
    // items; slices of course)
    cstr cs = cstr_utf8char(&c);
    size_t len = len_utf8char(c);
    for (size_t i = 0; i < len; i++) {
        push_Vec_char(&s->vec, cs[i]);
    }
}

/// Appends the given unicode codepoint to the end of this
/// String.

static UNUSED
void push_ucodepoint_String(String *s, ucodepoint c) {
    push_utf8char_String(s, new_utf8char_from_ucodepoint(c));
}



/* /// Appends the given cstr `cs` to the end of this String. `cs` is */
/* /// checked for correct UTF-8 encoding. */

/* static UNUSED */
/* Result(Unit, UnicodeError) push_cstr_String(String *s, cstr cs) { */
/*     BEGIN_Result(Unit, UnicodeError); */
    
/*     // XX 'cheap' route, stupid */
/*     CFile in = TRY(memopen_CFile((char*)s, strlen(s), "r"), cleanup1); */
    
/* cleanup2: */
/*     drop_CFile(in); */
/* cleanup1: */
/*     END_Result(); */
/* } */

// ^ not now. once separate, faster decoder is ready, then.


#include <cj50/instantiations/Result_size_t__UnicodeError.h>

/// Read all unicode codepoints into buf until the delimiter character
/// or EOF is reached.

/// This function will read characters from the underlying stream
/// until the delimiter or EOF is found. Once found, all characters up
/// to, and, unless `strip_delimiter` is true, including, the
/// delimiter (if found) will be appended to `buf`.

/// If successful, this function will return the total number of
/// characters read.

/// `max_len` is the maximum number of characters that will be
/// appended to `buf`. After this point, an error with `.kind ==
/// UnicodeErrorKind_LimitExceededError` is returned.

Result(size_t, UnicodeError) read_until_Vec_ucodepoint
    (CFile *in,
     ucodepoint delimiter,
     Vec(ucodepoint) *buf,
     bool strip_delimiter,
     size_t max_len)
{
    BEGIN_Result(size_t, UnicodeError);

    size_t nread = 0;
    while_let_Some(c, TRY(get_ucodepoint_unlocked(in), cleanup1)) {
        bool is_end = equal_ucodepoint(&c, &delimiter);
        bool needs_push = is_end ? (!strip_delimiter) : true;
        if (needs_push) {
            if (nread < max_len) {
                push_Vec_ucodepoint(buf, c);
                nread++;
            } else {
                RETURN_Err(UnicodeError_LimitExceeded, cleanup1);
            }
        }
    }
    RETURN_Ok(nread, cleanup1);
    
 cleanup1:
    END_Result();
}
