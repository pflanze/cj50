#pragma once

#include <cj50/gen/Option.h>

/// We alias this lengthy C type name to the shorter naming used in
/// the Rust programming language.

/// (This is especially helpful to preserve the use of "_" for
/// appending type names in combined types.)

typedef uint8_t u8;


static UNUSED
bool equal_u8(const u8 *a, const u8 *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_u8(const u8 a, const u8 b) {
    return a == b;
}

static UNUSED
void drop_u8(const u8 UNUSED a) {}

static UNUSED
int print_u8(u8 n) {
    return printf("%hhu", n);
}

static UNUSED
int print_debug_u8(const u8 *n) {
    return print_u8(*n);
}

GENERATE_Option(u8);

