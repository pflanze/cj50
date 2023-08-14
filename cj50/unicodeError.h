#pragma once


enum DecodingErrorKind {
    DecodingErrorKind_InvalidStartByte,
    DecodingErrorKind_PrematureEof,
    DecodingErrorKind_InvalidContinuationByte,
    DecodingErrorKind_InvalidCodepoint,
    // XX todo:
    // DecodingErrorKind_unexpected_continuation_byte, // same as DecodingErrorKind_InvalidStartByte?
    // non-continuation byte before the end of the character -> DecodingErrorKind_InvalidContinuationByte ?
    DecodingErrorKind_OverlongEncoding,
};

typedef struct DecodingError {
    enum DecodingErrorKind kind;
    union {
        int8_t byte_number;
        uint32_t codepoint; // would only need 21 bits, todo
    };
} DecodingError;

#define DecodingError_InvalidStartByte()              \
    ((DecodingError) {                                \
        .kind = DecodingErrorKind_InvalidStartByte    \
    })

#define _DecodingError_with_byte_number(_kind, bytenum)  \
    ((DecodingError) {                                   \
        .kind = (_kind),                                 \
        .byte_number = (bytenum)                         \
    })

#define DecodingError_PrematureEof(bytenum)                         \
    _DecodingError_with_byte_number(DecodingErrorKind_PrematureEof, \
                                    bytenum)

#define DecodingError_InvalidContinuationByte(bytenum)              \
    _DecodingError_with_byte_number(                                \
        DecodingErrorKind_InvalidContinuationByte,                  \
        bytenum)

#define DecodingError_InvalidCodepoint(cp)              \
    ((DecodingError) {                                  \
        .kind = DecodingErrorKind_InvalidCodepoint,     \
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
        && ((a->kind == DecodingErrorKind_InvalidStartByte)
            ||
            ((a->kind == DecodingErrorKind_PrematureEof) ?
             (a->byte_number == b->byte_number) :
             (a->kind == DecodingErrorKind_InvalidContinuationByte) ?
             (a->byte_number == b->byte_number) :
             (a->kind == DecodingErrorKind_InvalidCodepoint) ?
             (a->codepoint == b->codepoint) :
             die_match_failure()));
}

static UNUSED
int print_debug_DecodingError(const DecodingError *e) {
    INIT_RESRET;
    RESRET(printf("DecodingError("));
    switch (e->kind) {
    default: die_match_failure();

    case DecodingErrorKind_InvalidStartByte:
        RESRET(printf("DecodingErrorKind_InvalidStartByte"));
        break;        
    case DecodingErrorKind_PrematureEof:
        // well, this is proper ADT approach but not proper C syntax!
        // Except if I make macros. todo?
        RESRET(printf("DecodingErrorKind_PrematureEof(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_InvalidContinuationByte:
        RESRET(printf("DecodingErrorKind_InvalidContinuationByte(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_InvalidCodepoint:
        RESRET(printf("DecodingErrorKind_InvalidContinuationByte(%i)",
                      e->byte_number));
        break;
    case DecodingErrorKind_OverlongEncoding:
        // XX will it have some info?
        RESRET(printf("DecodingErrorKind_OverlongEncoding"));
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

    case DecodingErrorKind_InvalidStartByte:
        RESRET(fprintf(out, "invalid start byte"));
        break;
    case DecodingErrorKind_PrematureEof:
        RESRET(fprintf(out, "premature EOF decoding UTF-8 (byte #%i)",
                       e->byte_number));
        break;
    case DecodingErrorKind_InvalidContinuationByte:
        RESRET(fprintf(out, "invalid continuation byte (byte #%i)",
                       e->byte_number));
        break;
    case DecodingErrorKind_InvalidCodepoint:
        RESRET(fprintf(out, "invalid code point %ui",
                       e->codepoint));
        break;
    case DecodingErrorKind_OverlongEncoding:
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
    UnicodeErrorKind_DecodingError,
    UnicodeErrorKind_LimitExceededError,
    UnicodeErrorKind_ExpectedOneCodepointError,
};

typedef struct UnicodeError {
    enum UnicodeErrorKind kind;
    union {
        SystemError systemError;
        DecodingError decodingError;
        // LimitExceededError -;
        // ExpectedOneCodepointError -;
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

#define UnicodeError_LimitExceeded                      \
    ((UnicodeError) {                                   \
        .kind = UnicodeErrorKind_LimitExceededError     \
    })

#define UnicodeError_ExpectedOneCodepoint                      \
    ((UnicodeError) {                                          \
        .kind = UnicodeErrorKind_ExpectedOneCodepointError     \
    })


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
            (a->kind == UnicodeErrorKind_LimitExceededError) ?
            true :
            (a->kind == UnicodeErrorKind_ExpectedOneCodepointError) ?
            true :
            die_invalid_UnicodeErrorKind(a->kind));
}

static UNUSED
int print_debug_UnicodeError(const UnicodeError *e) {
    INIT_RESRET;
    RESRET(printf("UnicodeError("));
    switch (e->kind) {
    default: die_match_failure();

    case UnicodeErrorKind_SystemError:
        RESRET(print_debug_SystemError(&e->systemError));
        break;
    case UnicodeErrorKind_DecodingError:
        RESRET(print_debug_DecodingError(&e->decodingError));
        break;
    case UnicodeErrorKind_LimitExceededError:
        RESRET(print_move_cstr("LimitExceededError"));
        break;
    case UnicodeErrorKind_ExpectedOneCodepointError:
        RESRET(print_move_cstr("ExpectedOneCodepointError"));
        break;
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
    case UnicodeErrorKind_LimitExceededError:
        // XX this could have more information? (Also, context?)
        return fprintln_move_cstr(out, "Error: a limit was exceeded");
    case UnicodeErrorKind_ExpectedOneCodepointError:
        // XX this could have more information? (Also, context?)
        return fprintln_move_cstr(out, "Error: expected a single unicode codepoint");
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
    case UnicodeErrorKind_LimitExceededError:
        break;
    case UnicodeErrorKind_ExpectedOneCodepointError:
        break;
    }
}

