#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <cj50/macro-util.h>
#include <cj50/xmem.h>
#include <cj50/gen/Option.h>
#include <cj50/gen/Result.h>
#include <cj50/Unit.h>
#include <cj50/CStr.h> /* int print_move_cstr(cstr s) */
#include <cj50/gen/error.h>

#define Vec(T) XCAT(Vec_,T)


// XX Bummer, why necessary, circular dependencies?
typedef const char* cstr;
int print_move_cstr(cstr s);


// ------------------------------------------------------------------
// Errors
// XX ~copy-paste from CStr.h, should generate such, too.

/// A type indicating an error handling Vec. The instances are:

///     VecError_out_of_capacity

typedef struct VecError {
    uint8_t code;
} VecError;

#define VecError(cod) ((VecError) { .code = cod })

/// Check equivalence.
static UNUSED
bool equal_VecError(const VecError *a, const VecError *b) {
    return a->code == b->code;
}

#define DEF_VecError(code, name) const VecError name = VecError(code)

DEF_VecError(0, VecError_out_of_capacity);

const struct constant_name_and_message constant_name_and_message_from_VecError_code[] = {
    { "VecError_out_of_capacity", "Vec is out of capacity to push more items" },
};
#define constant_name_and_message_from_VecError_code_len        \
    (sizeof(constant_name_and_message_from_VecError_code)       \
     / sizeof(struct constant_name_and_message))

#undef DEF_VecError
// Make final (well)
#undef VecError

/// Print in C code syntax.
static UNUSED
int print_debug_VecError(const VecError *e) {
    // (We said final, and now we're printing syntax using the constructor!...)
    assert(e->code < constant_name_and_message_from_VecError_code_len);
    return printf("VecError(%s)",
                  constant_name_and_message_from_VecError_code[e->code].constant_name);
}

/// Print for program user.
static UNUSED
int fprintln_VecError(FILE *out, const VecError *e) {
    assert(e->code < constant_name_and_message_from_VecError_code_len);
    return fprintf(
        out, "Vec error: %s\n",
        constant_name_and_message_from_VecError_code[e->code].message);
}

static UNUSED
void drop_VecError(UNUSED VecError e) {}

// ------------------------------------------------------------------

GENERATE_Result(Unit, VecError);


/// Return the larger of the two arguments.
static UNUSED
size_t max_size_t(size_t a, size_t b) {
    return a > b ? a : b;
}

// template: see cj50/gen/template/Vec.h 

