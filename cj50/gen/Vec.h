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

// For the template/Vec.h:
#include <cj50/resret.h>

//! The `cj50/gen/Vec` library is implementing vectors (mutable
//! ordered collections with O(1) access and ability to change size at
//! the end) and slices (immutable collections with O(1) read access)
//! parameterized with the item type, and is implemented in the following
//! files:

//! * `cj50/gen/Vec.h` (main file, non-parameterized parts)
//! * [`cj50/gen/template/Vec.h`](template/Vec.h.md) (parameterized parts instantiated once per Vec)
//! * [`cj50/gen/template/slices.h`](template/slices.h.md) (parameterized parts instantiated  for slice, mutslice)
//! * [`cj50/gen/template/vectorlikes.h`](template/vectorlikes.h.md) (parameterized parts instantiated for Vec, slice, mutslice)

//! Vectors and slices are related: a `Vec` is an owned data structure
//! (it must reside in exactly one place at any time), whereas both
//! `slice` and its mutable sibling `mutslice` are borrowed (they must
//! only be used as long as the holder of the storage, either a `Vec`,
//! or static storage (which exists for the duration of the program
//! and hence is unproblematic), is available). `Vec` allows both
//! replacing existing elements, as well as changing its size (via
//! `push` and `pop` and (in the future) other functions), which is
//! only allowed when there are no `slices` of it in use. `mutslice`
//! can only be borrowed from `Vec` (not constant storage), and allows
//! replacing its elements, which replaces them for the `Vec` they are
//! borrowed from, too; but it cannot change size (there are no `push`
//! or similar operations). `slice` cannot be modified at all, but in
//! exchange any number of `slice`s can exist at the same time as long
//! as there is no `mutslice` and `Vec` is left untouched.

//! Vectors and slices are cj50's replacement for pointers to arrays
//! which are the basic building blocks for sequences of items in C,
//! but which are prone to mistaken use. The vectors and slices here
//! are very closely modelled after their counterparts in Rust.


#define Vec(T) XCAT(Vec_,T)

#define slice(T) XCAT(slice_,T)

#define mutslice(T) XCAT(mutslice_,T)

/// Create a slice with its storage on the stack.

/// Example:
/// 
/// ```C
/// DEF_SLICE(ColorFunction_float, sl, {
///     { color(255, 0, 0), f },
///     { color(0, 255, 0), g }
/// });
/// ```

#define DEF_SLICE(T, var, ...)                                          \
    T HYGIENIC(val)[] = __VA_ARGS__;                                    \
    AUTO var = XCAT(new_slice_, T)(HYGIENIC(val),                       \
                                  sizeof(HYGIENIC(val)) / sizeof(T));   \


/// Create a slice from an array variable with static size.

/// NOTE: requires up to date `new_slice` generic.

/// Example:
/// 
/// ```C
/// ColorFunction_float fs[] = {
///     { color(240, 220, 0), f }
/// };

/// foo(SLICE_FROM_ARRAY(fs));
/// ```

#define SLICE_FROM_ARRAY(var)                   \
    new_slice(var, sizeof(var) / sizeof(var[0]))


/// Create a slice from an array variable with static size.

/// (Does not require `new_slice` generic, but needs type name
/// argument.)

/// Example:
/// 
/// ```C
/// ColorFunction_float fs[] = {
///     { color(240, 220, 0), f }
/// };

/// foo(SLICE_FROM_ARRAY_OF_T(ColorFunction_float, fs));
/// ```

#define SLICE_FROM_ARRAY_OF_T(T, var)                   \
    XCAT(new_slice_, T)(var, sizeof(var) / sizeof(T))


// XX Bummer, why necessary, circular dependencies?
typedef const char* cstr;
static UNUSED
int print_move_cstr(cstr s);


// ------------------------------------------------------------------
// Errors
// XX ~copy-paste from CStr.h, should generate such, too.

/// A type indicating an error handling Vec. The instances are:

///     VecError_OutOfCapacity

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

DEF_VecError(0, VecError_OutOfCapacity);

const struct constant_name_and_message constant_name_and_message_from_VecError_code[] = {
    { "VecError_OutOfCapacity", "Vec is out of capacity to push more items" },
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

