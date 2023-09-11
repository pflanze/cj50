#pragma once

#include <cj50/gen/Option.h>

/// We alias this lengthy C type name to the shorter naming used in
/// the Rust programming language.

/// (This is especially helpful to preserve the use of "_" for
/// appending type names in combined types.)

typedef int64_t i64;


static UNUSED
bool equal_i64(const i64 *a, const i64 *b) {
    return *a == *b;
}

static UNUSED
bool equal_move_i64(const i64 a, const i64 b) {
    return a == b;
}

static UNUSED
void drop_i64(const i64 UNUSED a) {}

static UNUSED
int print_i64(const i64 *n) {
    return printf("%lu", *n);
}

static UNUSED
int print_move_i64(i64 n) {
    return print_i64(&n);
}

static UNUSED
int print_debug_i64(const i64 *n) {
    return print_i64(n);
}

static UNUSED
int print_debug_move_i64(i64 n) {
    return print_debug_i64(&n);
}

GENERATE_Option(i64);

