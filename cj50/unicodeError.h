#pragma once


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
    UnicodeErrorKind_DecodingError,
    UnicodeErrorKind_LimitExceededError,
};

typedef struct UnicodeError {
    enum UnicodeErrorKind kind;
    union {
        SystemError systemError;
        DecodingError decodingError;
        // LimitExceededError -;
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
    }
}

