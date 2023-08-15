#pragma once

#include <cj50/gen/Option.h>

/// We alias this lengthy C type name to the shorter naming used in
/// the Rust programming language.

/// (This is especially helpful to preserve the use of "_" for
/// appending type names in combined types.)

typedef uint64_t u64;


static UNUSED
bool equal_u64(const u64 *a, const u64 *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_u64(const u64 a, const u64 b) {
    return a == b;
}

static UNUSED
void drop_u64(const u64 UNUSED a) {}

static UNUSED
int print_u64(const u64 *n) {
    return printf("%lu", *n);
}

static UNUSED
int print_move_u64(u64 n) {
    return print_u64(&n);
}

static UNUSED
int print_debug_u64(const u64 *n) {
    return print_u64(n);
}

static UNUSED
int print_debug_move_u64(u64 n) {
    return print_debug_u64(&n);
}

GENERATE_Option(u64);

