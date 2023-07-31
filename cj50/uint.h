#pragma once

#include <cj50/gen/Option.h>

/// An integer number type that cannot represent negative numbers, but
/// instead has a little more room in the positive number range than
/// `int` (0 .. `UINT_MAX`).

/// CAUTION: arithmetic with numbers of this type does wrap around if
/// the result of the arithmetic operation is larger than the largest
/// number that can be represented with this type, or smaller than
/// 0. This is unlike arithmetic with `int`, where such overflows are
/// an error and (with the compiler options that we use) abort the
/// program. So it is best to avoid `uint` and always use `int`
/// instead, except when using functions from libraries or the
/// operating system that use this type.

typedef unsigned int uint;

int print_uint(uint n) {
    return printf("%u", n);
}


static UNUSED
bool equal_uint(const uint *a, const uint *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_uint(const uint a, const uint b) {
    return a == b;
}

static UNUSED
void drop_uint(const uint UNUSED a) {}

static UNUSED
uint print_debug_uint(const uint *n) {
    return printf("%i", *n);
}

GENERATE_Option(uint);

