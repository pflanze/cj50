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
#include <cj50/instantiations/Result_Unit__UnicodeError.h>
#include <cj50/xmem.h>


static UNUSED
NORETURN die_bug_unicode() {
    DIE("bug in unicode.h");
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

// utf8_sequence_len_ucodepoint(ucodepoint cp) see further down.


/// How many bytes the UTF-8 character sequence takes when `b` is its
/// initial byte. None is returned if `b` is not ascii or an initial
/// byte, but a continuation byte or invalid.

static UNUSED
Option(u8) utf8_sequence_len_u8(u8 b) {
    if (b <= 0x7F) { return some_u8(1); }
    if ((b & 0b11100000) == 0b11000000) { return some_u8(2); }
    if ((b & 0b11110000) == 0b11100000) { return some_u8(3); }
    if ((b & 0b11111000) == 0b11110000) { return some_u8(4); }
    return none_u8();
}

static UNUSED
bool is_utf8_continuation_byte(u8 b) {
    return (b & 0b11000000) == 0b10000000;
}


// ------------------------------------------------------------------

/// A single Unicode code point (a single character, unless it's a
/// code point that combines with other code points to a composed
/// character). ucodepoint only ever carries valid codes.

typedef struct ucodepoint {
    uint32_t u32; // would only need 21 bits
} ucodepoint;

// ^ Should this be called UCodepoint ? It's a Copy type, but so what?

/// Unsafe constructor, does not verify the code. Can be evaluated in
/// the toplevel.
#define ucodepoint(cp) \
    ((ucodepoint) { .u32 = (cp) })

/// Safe constructor, does decode str and verify its validity and that
/// there is only one codepoint and aborts (via unwrap) if not. Cannot
/// be evaluated in the toplevel.
#define uchar(str)                              \
    unwrap_Result_ucodepoint__UnicodeError(ucodepoint_from_cstr(str))


static UNUSED
void drop_ucodepoint(UNUSED ucodepoint c) {}

static UNUSED
bool equal_ucodepoint(const ucodepoint *a, const ucodepoint *b) {
    return a->u32 == b->u32;
}

static UNUSED
bool equal_move_ucodepoint(ucodepoint a, ucodepoint b) {
    return a.u32 == b.u32;
}

static UNUSED
int print_debug_ucodepoint(const ucodepoint *a) {
    INIT_RESRET;
    RESRET(print_move_cstr("ucodepoint("));
    RESRET(print_move_u32(a->u32));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}

static UNUSED
int print_debug_move_ucodepoint(ucodepoint a) {
    return print_debug_ucodepoint(&a);
}

// print_ucodepoint, print_move_ucodepoint see further down


/// How many bytes the UTF-8 character sequence for unicode codepoint
/// `cp` takes (1..4). Since `ucodepoint` is guaranteed to be a
/// unicode codepoint, no errors are possible.

static UNUSED
int utf8_sequence_len_ucodepoint(ucodepoint cp) {
    if (cp.u32 <= 0x7F) {
        return 1;
    }
    if (cp.u32 <= 0x7FF) {
        return 2;
    }
    if (cp.u32 <= 0xFFFF) {
        return 3;
    }
    if (cp.u32 <= 0x10FFFF) {
        return 4;
    }
    die_bug_unicode();
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
size_t len_utf8char(const utf8char *c) {
    return c->data[5];
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
    return ((len_utf8char(a) == len_utf8char(b)) &&
            memcmp(a->data, b->data, len_utf8char(a)) == 0);
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

static UNUSED
int print_debug_move_utf8char(utf8char c) {
    return print_debug_utf8char(&c);
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

static UNUSED
int print_ucodepoint(const ucodepoint *a) {
    INIT_RESRET;
    utf8char uc = new_utf8char_from_ucodepoint(*a);
    RESRET(fwrite(cstr_utf8char(&uc), 1, len_utf8char(&uc), stdout));
cleanup:
    return ret;
}

static UNUSED
int print_move_ucodepoint(ucodepoint a) {
    return print_ucodepoint(&a);
}


// ------------------------------------------------------------------


GENERATE_Option(ucodepoint);

GENERATE_Result(Option(ucodepoint), UnicodeError);


/// Read a single Unicode code point from the given `CFile`.

/// This function is currently hard-coded to decode files in the UTF-8
/// format.

static UNUSED
Result(Option(ucodepoint), UnicodeError) get_ucodepoint_unlocked_CFile(CFile *in)
{
#define getc_unlocked(in) os_getc_unlocked(in)
#define POSSIBLYTRY(expr, label) TRY(expr, label)
#define POSSIBLYDEREF(v) v
#include "cj50/gen/template/unicode_utf8decode.h"
#undef POSSIBLYDEREF
#undef POSSIBLYTRY
#undef getc_unlocked
}


#include <cj50/instantiations/SliceIterator_char.h>

/// Read a single Unicode code point from the given `CFile`.

/// This function is currently hard-coded to decode files in the UTF-8
/// format.

static UNUSED
Result(Option(ucodepoint), UnicodeError) get_ucodepoint_unlocked_SliceIterator_char(
    SliceIterator(char) *in)
{
#define getc_unlocked(in) next_SliceIterator_char(in)
#define POSSIBLYTRY(expr, label) expr
#define POSSIBLYDEREF(v) (*(v))
#include "cj50/gen/template/unicode_utf8decode.h"
#undef POSSIBLYDEREF
#undef POSSIBLYTRY
#undef getc_unlocked
}

// ------------------------------------------------------------------
// Operations for String

/// Get the character (unicode codepoint, to be precise) at byte
/// position `idx` of `s`, if possible. Failures can be because `idx`
/// is at or behind the end of the string contents, or because it does
/// not point to the beginning of a byte sequence for a UTF-8 encoded
/// codepoint.

/// DEPRECATED, use get_ucodepoint_String instead.

static UNUSED
Option(utf8char) get_utf8char_String(const String *s, size_t idx) {
    size_t len = s->vec.len;
    char *ptr = s->vec.ptr;
    if (idx < len) {
        if_let_Some(seqlen, utf8_sequence_len_u8(ptr[idx])) {
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

/// Convert a slice of characters into a vector of unicode codepoints, if possible.
/// Conversion failures due to invalid UTF-8 are reported.

static UNUSED
Result(Vec(ucodepoint), UnicodeError) new_Vec_ucodepoint_from_slice_char(slice(char) s)
{
    BEGIN_Result(Vec(ucodepoint), UnicodeError);

    AUTO in = new_SliceIterator_char(s);
    Vec(ucodepoint) v = new_Vec_ucodepoint();
    while_let_Some(c, TRY(get_ucodepoint_unlocked_SliceIterator_char(&in), cleanup)) {
        push_Vec_ucodepoint(&v, c);
    }
    RETURN_Ok(v, cleanup);
cleanup:
    drop_SliceIterator_char(in);
    END_Result();
}

/// Convert a `cstr` into a vector of unicode codepoints, if possible.
/// Conversion failures due to invalid UTF-8 are reported.

static UNUSED
Result(Vec(ucodepoint), UnicodeError) new_Vec_ucodepoint_from_cstr(cstr s)
{
    return new_Vec_ucodepoint_from_slice_char(new_slice_char(s, strlen(s)));
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
    size_t len = len_utf8char(&c);
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



/// Appends the given cstr `cs` to the end of this String. `cs` is
/// checked for correct UTF-8 encoding.

static UNUSED
Result(Unit, UnicodeError) push_cstr_String(String *s, cstr cs) {
    BEGIN_Result(Unit, UnicodeError);

    AUTO slice = new_slice_char(cs, strlen(cs));
    AUTO in = new_SliceIterator_char(slice);
    while_let_Some(cp, TRY(get_ucodepoint_unlocked_SliceIterator_char(&in), cleanup1)) {
        utf8char c = new_utf8char_from_ucodepoint(cp);
        size_t len = len_utf8char(&c);
        for (size_t i = 0; i < len; i++) {
            push_String(s, c.data[i]);
        }
    }
    RETURN_Ok(Unit(), cleanup1);

cleanup1:
    drop_SliceIterator_char(in);
    END_Result();
}


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
    while_let_Some(c, TRY(get_ucodepoint_unlocked_CFile(in), cleanup1)) {
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


#include <cj50/instantiations/Result_ucodepoint__UnicodeError.h>

/// Return the single ucodepoint in `s`, if possible, returning
/// decoding errors as well when there are fewer or more than 1
/// ucodepoint in `s`.

static UNUSED
Result(ucodepoint, UnicodeError) ucodepoint_from_cstr(cstr s) {
    BEGIN_Result(ucodepoint, UnicodeError);
    if (s[0] == '\0') {
        // cstr can't represent \0, so it's the end of string
        RETURN_Err(UnicodeError_ExpectedOneCodepoint, cleanup1);
    }
    if_let_Some(slen, utf8_sequence_len_u8(s[0])) {
        size_t len = strlen(s);
        if (len == slen) {
            AUTO iter = new_SliceIterator_char(new_slice_char(s, len));
            RETURN_Ok(unwrap_Option_ucodepoint(
                          TRY(get_ucodepoint_unlocked_SliceIterator_char(&iter),
                              cleanup1)),
                      cleanup1);
        } else {
            RETURN_Err(UnicodeError_ExpectedOneCodepoint, cleanup1);
        }
    } else_None {
        RETURN_Err(DecodingError_InvalidStartByte(), cleanup1);
    }

cleanup1:
    END_Result();
}


/// Get the character (unicode codepoint, to be precise) at byte
/// position `idx` of `s`, if possible. Failures can be because `idx`
/// is at or behind the end of the string contents, or because it does
/// not point to the beginning of a byte sequence for a UTF-8 encoded
/// codepoint.

static UNUSED
Option(ucodepoint) get_ucodepoint_String(const String *s, size_t idx) {
    AUTO iter = new_SliceIterator_char(unsafe_slice_of_String(
                                           s, range(idx, s->vec.len)));
    if_let_Ok(opt_cp, get_ucodepoint_unlocked_SliceIterator_char(&iter)) {
        return opt_cp;
    } else_Err(UNUSED _) {
        return none_ucodepoint();
    } end_let_Ok;
}


/// Get a slice of the string. Note that the given range of indices
/// must be in byte positions, not unicode codepoints. Checks are done
/// that the given positions are at UTF-8 boundaries and not beyond
/// the end of the string, otherwise None is returned.

static UNUSED
Option(slice(char)) get_slice_of_String(const String *s, Range idx) {
    if (!(idx.start <= idx.end)) {
        return none_slice_char();
    }
    size_t len = s->vec.len;
    if (!(idx.end <= len)) {
        return none_slice_char();
    }
    if (idx.end < len) {
        if (is_utf8_continuation_byte(s->vec.ptr[idx.end])) {
            return none_slice_char();
        }
    }
    if (is_utf8_continuation_byte(s->vec.ptr[idx.start])) {
        return none_slice_char();
    }
    return some_slice_char(unsafe_slice_of_String(s, idx));
}



/// Read all unicode codepoints into buf until `uchar("\n")` or EOF is
/// reached. `strip_delimiter` and `max_len` have the same meaning as
/// for `read_until_Vec_ucodepoint`.

Result(size_t, UnicodeError) read_line_Vec_ucodepoint
    (CFile *in,
     Vec(ucodepoint) *buf,
     bool strip_delimiter,
     size_t max_len)
{
    return read_until_Vec_ucodepoint(in, uchar("\n"), buf, strip_delimiter, max_len);
}


/// The number of unicode code points in the given slice.

static UNUSED
Result(size_t, UnicodeError) ucodepoint_count_slice_char(slice(char) s) {
    // NOTE: count(ucodepoint_iter(s)) would be the right approach in
    // the FUTURE instead!
    BEGIN_Result(size_t, UnicodeError);

    AUTO iter = new_SliceIterator_char(s);
    size_t count = 0;
    while_let_Some(_, TRY(get_ucodepoint_unlocked_SliceIterator_char(&iter),
                          cleanup1)) {
        drop_ucodepoint(_);
        count++;
    }
    RETURN_Ok(count, cleanup1);

cleanup1:
    drop_SliceIterator_char(iter); // (even though it's a noop)
    END_Result();
}

/// Whether the given slice represents valid and canonically UTF-8
/// encoded unicode codepoints.

static UNUSED
bool is_valid_utf8_slice_char(slice(char) s) {
    if_let_Ok(UNUSED _, ucodepoint_count_slice_char(s)) {
        return true;
    } else_Err(UNUSED _) {
        return false;
    } end_let_Ok;
}


/// Create a String from a CStr, consuming the latter.

/// Asserts that CStr is in correct UTF-8 encoding, just aborts if not.

static UNUSED
String new_String_from_CStr(CStr s) {
    size_t len = strlen(s.cstr);
    assert(is_valid_utf8_slice_char(new_slice_char(s.cstr, len)));
    return (String) {
        .vec = (Vec(char)) {
            .ptr = s.cstr,
            .cap = len + 1,
            .len = len
        }       
    };
}

/// Create a String from a char slice, copying the data in the slice.

/// Asserts that the slice is in correct UTF-8 encoding, just aborts
/// if not.

static UNUSED
String new_String_from_slice_char(slice(char) s) {
    assert(is_valid_utf8_slice_char(s));
    return (String) {
        .vec = (Vec(char)) {
            .ptr = xmemcpy(s.ptr, s.len + 1),
            .cap = s.len + 1,
            .len = s.len
        }       
    };
}

/// Create a String from a cstr, copying the data in the string.

/// Asserts that the string is in correct UTF-8 encoding, just aborts
/// if not.

static UNUSED
String new_String_from_cstr(cstr s) {
    return new_String_from_slice_char(new_slice_char(s, strlen(s)));
}


// Redefine new_from now that new_String_from_cstr exists... (horrible)
#undef NEW_FROM_STAGE
#define NEW_FROM_STAGE 2
#undef NEW_FROM_h
#include <cj50/gen/dispatch/new_from.h>

