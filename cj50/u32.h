#pragma once

#include <cj50/gen/Option.h>

/// We alias this lengthy C type name to the shorter naming used in
/// the Rust programming language.

/// (This is especially helpful to preserve the use of "_" for
/// appending type names in combined types.)

typedef uint32_t u32;


static UNUSED
bool equal_u32(const u32 *a, const u32 *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_u32(const u32 a, const u32 b) {
    return a == b;
}

static UNUSED
void drop_u32(const u32 UNUSED a) {}

static UNUSED
int print_u32(const u32 *n) {
    return printf("%u", *n);
}

static UNUSED
int print_move_u32(u32 n) {
    return print_u32(&n);
}

static UNUSED
int print_debug_u32(const u32 *n) {
    return print_u32(n);
}

static UNUSED
int print_move_debug_u32(u32 n) {
    return print_debug_u32(&n);
}

GENERATE_Option(u32);

