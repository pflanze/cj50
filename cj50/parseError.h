#pragma once

#include <cj50/CStr.h>
#include <cj50/int.h>
#include <cj50/gen/Result.h>
#include <cj50/String.h>
#include <cj50/resret.h>
#include <cj50/basic-util.h>
#include <cj50/xmem.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/// The type for error codes in `ParseError`.

/// Currently, codes in the range 1..255 represent errors defined by
/// the operating system (errno), codes in the range 500..29999 are
/// specific to cj50.h. But do *not* rely on any details of this type,
/// they may change, values of this type should just be passed to one
/// of the functions that accept it (`string_from_ParseError` and
/// `print_ParseError`).

typedef uint16_t ParseError__code_t;

/// Values of this type describe the reason why a string does not
/// contain text that properly represents a value that the used parse
/// function should return.

typedef struct ParseError {
    ParseError__code_t code;
} ParseError;

// The stylistic reason for not using `enum` is that the type is used
// for other values, too.
const ParseError__code_t E_not_in_int_range = 500;
const ParseError__code_t E_invalid_text_after_number = 501;
const ParseError__code_t E_not_greater_than_zero = 502;
const ParseError__code_t E_negative = 503;
const ParseError__code_t E_not_a_number = 504;


#define ParseError(e) ((ParseError) { .code = (e) })

static
bool equal_ParseError(const ParseError* a, const ParseError* b) {
    return a->code == b->code;
}

static
int print_debug_ParseError(const ParseError *self) {
    INIT_RESRET;
    RESRET(print_move_cstr("ParseError("));
    RESRET(print_move_int(self->code));
    RESRET(print_move_cstr(")"));
cleanup:
    return ret;
}


/// Convert a `ParseError` value into a `CStr` for display.
static
CStr string_from_ParseError(const ParseError *e) {
    if (e->code == E_not_in_int_range) {
        return CStr_from_cstr_unsafe(
            xstrdup("is not within the range of numbers of the `int` type"));
    } else if (e->code == E_invalid_text_after_number) {
        return CStr_from_cstr_unsafe(
            xstrdup("has invalid text after the number"));
    } else if (e->code == E_not_greater_than_zero) {
        return CStr_from_cstr_unsafe(
            xstrdup("is not greater than zero"));
    } else if (e->code == E_negative) {
        return CStr_from_cstr_unsafe(
            xstrdup("is negative"));
    } else if (e->code == E_not_a_number) {
        return CStr_from_cstr_unsafe(
            xstrdup("is not a number"));
    } else if (e->code < 256) {
#define SIZ_ 200
        CStr s = new_CStr(SIZ_);
        assert(snprintf(s.cstr, SIZ_,
                        "is not valid: %s", strerror(errno)) < SIZ_);
        return s;
#undef SIZ_
    } else {
        DIE("BUG: invalid ParseError value");
    }
}

static UNUSED
int print_ParseError(const ParseError *e) {
    INIT_RESRET;
    Option(CStr) s = NONE;
    RESRET(print_move_cstr("parse error: "));
    s = some_CStr(string_from_ParseError(e));
    RESRET(print_CStr(&s.value));
cleanup:
    drop_Option_CStr(s);
    return ret;
}

static
int fprintln_ParseError(FILE* out, const ParseError *e) {
    INIT_RESRET;
    CStr s = string_from_ParseError(e);
    RESRET(fprintf(out, "parse error: input %s\n", s.cstr));
cleanup:
    drop_CStr(s);
    return ret;
}

static UNUSED
String new_String_from_ParseError(ParseError e) {
    // XX finally provide a macro to do this or something
    char *str = NULL;
    UNUSED size_t strlen = 0;
    FILE *fh = open_memstream(&str, &strlen);
    assert(fprintln_ParseError(fh, &e) >= 0);
    fclose(fh);
    String s = new_String_from_move_cstr(str);
    free(str);
    return s;
}

static UNUSED
void drop_ParseError(UNUSED ParseError e) {}


GENERATE_Result(int, ParseError);

