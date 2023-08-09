/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

#pragma once

#include <cj50/gen/Result.h>
#include <cj50/gen/Option.h>
#include <cj50/CStr.h>
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

